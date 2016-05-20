#include "player.h"

#include "common/log.h"

// Average male dimensions
#define PLAYER_CAPSULE_HEIGHT 1.73
#define PLAYER_CAPSULE_RADIUS 0.40f

// 78 kg mass person
#define PLAYER_MASS 78.0f

namespace redc
{
  Player_Controller::Player_Controller() : inited_(false), ghost_(),
                                           shape_(PLAYER_CAPSULE_RADIUS,
                                                  PLAYER_CAPSULE_HEIGHT),
                                           jump_velocity_() {}

  // They are expected to add the action to the dynamics world and then this
  // will be called, then we init then we start
  void Player_Controller::updateAction(btCollisionWorld* world, btScalar dt)
  {
    if(!inited_)
    {
      // Initialize velocity and impulse to zero
      jump_velocity_.setZero();

      last_normal_.setZero();

      // Set it to a unit vector going straight upwards
      last_normal_.setY(1.0f);

      // Player state
      state = Player_State::Grounded;

      // Transform
      btTransform trans;
      trans.setIdentity();
      trans.setOrigin(btVector3(0.0f, 4.5f, 0.0f));
      ghost_.setWorldTransform(trans);

      // Shape
      ghost_.setCollisionShape(&shape_);

      // Flags
      ghost_.setCollisionFlags(ghost_.getCollisionFlags() |
                               btCollisionObject::CF_KINEMATIC_OBJECT);
      ghost_.setActivationState(DISABLE_DEACTIVATION);

      // Ghost callback, this updates any ghost object with any and all objects
      // it collides with so later on we can collision dispatching on only those
      // pairs (and get contacts back, etc).

      // **Important:** Add this before adding the ghost object, otherwise the
      // ghost object will not get notified and dispatching collisions of the
      // ghost object will literally ignore all collisions with the world. This
      // actually bit me in the butt before these two statements were
      // re-ordered. Interesting because even though the objects were actually
      // colliding, the (pair caching) ghost object was not dispatching the
      // ghost-map collisions.
      world->getPairCache()->setInternalGhostPairCallback(&ghost_cb_);

      // Add to world
      world->addCollisionObject(&ghost_);

      // Set the pitch to an identity rotation.
      pitch_.setEuler(0.0f, 0.0f, 0.0f);

      inited_ = true;
      return;
    }

    // Current world transform
    auto xform = ghost_.getWorldTransform();
    // Current position
    auto pos = xform.getOrigin();

    // Updates the ghost object. Suffixed with an underscore so we know it's
    // private... to this function.
    auto update_ghost_transform_ = [&]()
    {
      xform.setOrigin(pos);
      ghost_.setWorldTransform(xform);
    };

    // ===
    // Ray tracing floor-collision handling
    // ===
    {
      // First trace a ray from the player to the near-ground. If we hit
      // something we should consider the player on the ground, that means we no
      // longer have to apply gravity, for example.

      // How far should the ray extend past the bottom of the ghost object.
      constexpr float shoe_size = 0.7f;

      auto start_pos = pos;
      start_pos.setY(pos.getY() - PLAYER_CAPSULE_HEIGHT / 2.0f);

      auto end_pos = pos;
      end_pos.setY(start_pos.getY() - shoe_size);

      // Redundancy whhyyyyy
      btCollisionWorld::ClosestRayResultCallback ground_ray(start_pos, end_pos);
      world->rayTest(start_pos, end_pos, ground_ray);

      if(ground_ray.hasHit())
      {
        // What is the player doing?

        // Notes:

        // This section used to reset the player's y position to the ray
        // intersection point, the issue is that it would interfere with the
        // penetration recovery and cause a constant switch from the player
        // being grounded to being registered as jumping. The ray trace ended up
        // locking it too close to the ground and then the recover would push it
        // up too far such that next frame we switched to jumping, etc.

        // Anyways it works better now because we still use the player state to
        // decide whether or not to apply gravity but we don't accidentally move
        // the player and cause strange unexpected changes. The penetration
        // recovery works well enough that it can handle moving the player above
        // the floor just fine. After that, as long as gravity isn't applied the
        // player remains stable.

        // One issue that I'm seeing now that due to a large value for
        // shoe_size, the player can get stuck low down in the range of
        // allowable heights and then a jump becomes ineffective because even
        // after that one tick of movement the player is still in the allowable
        // range, which means the player is immediately considered to be on the
        // ground. Of course, currently that means velocity is not applied.
        if(state == Player_State::Jumping) state = Player_State::Grounded;

        last_normal_ = ground_ray.m_hitNormalWorld;
      }

      // No ground?
      else
      {
        if(state == Player_State::Grounded) state = Player_State::Jumping;
      }

      update_ghost_transform_();
    }

    // ===
    // Handle input
    // ===

    {
      btVector3 local_dpos;
      local_dpos.setZero();

      if(input_ref_->forward)
      {
        local_dpos.setZ(-1.0f);
      }
      if(input_ref_->backward)
      {
        local_dpos.setZ(+1.0f);
      }
      if(input_ref_->strafe_left)
      {
        local_dpos.setX(-1.0f);
      }
      if(input_ref_->strafe_right)
      {
        local_dpos.setX(+1.0f);
      }

      if(input_ref_->jump && state == Player_State::Grounded)
      {
        // If we are on the ground, apply an impulse to the player upward (ie
        // change the velocity).
        jump_velocity_ = btVector3(0.0f, 6.0f, 0.0f);
        state = Player_State::Jumping;
      }
      else if(input_ref_->jump && state == Player_State::Flying)
      {
        // If we were in the air, just release the player from the grappling
        // hook (ie go into jumping mode, where gravity will be applied).
        // Velocity will not be zero'd so it should be interesting.
        state = Player_State::Jumping;
      }

      // The active normal is the normal of the ground beneath us, when we are
      // not on the ground it's just the y axis. This is the normal the player
      // moves perpendicular to when using the WASD keys, etc.
      auto active_normal = last_normal_;
      if(state != Player_State::Grounded)
      {
        active_normal.setZero();
        active_normal.setY(1.0f);
      }

      // No movement; no problem!
      if(FLT_EPSILON < btFabs(local_dpos.length()))
      {
        // We only really care about the direction in the x and z direction.
        // The y direction we will find ourselves such that the final movement
        // is perpendicular to the surface normal.
        auto movement = btMatrix3x3(xform.getRotation()) * local_dpos;

        // We want our final movement vector to be perpendicular to the normal
        // but it also has to be the same x and z value of the movement we
        // calculated before.
        auto partial_dot = active_normal.getX() * movement.getX() +
                           active_normal.getZ() * movement.getZ();

        // Use the fact that movement dot normal should be zero
        // 0 = partial_dot + new_movement_y * active_normal.getY();
        // -partial_dot / active_normal.getY() = new_movement_y;
        movement.setY(-partial_dot / active_normal.getY());

        // Normalize and scale
        movement.normalize() *= .1f;

        // Cast a ray from the character's position to the position plus the
        // movement vector. This is obviously where the player is trying to go.
        auto end_pt = pos + movement.normalized() * PLAYER_CAPSULE_RADIUS * 4.5f;
        btCollisionWorld::ClosestRayResultCallback move_ray(pos, end_pt);
        world->rayTest(pos, end_pt, move_ray);

        if(move_ray.hasHit())
        {
          auto normal = move_ray.m_hitNormalWorld;

          // Find the perpendicular movement along the normal in the direction
          // of movement.

          // Movement T Normal = (Normal x Movement) x Normal
          auto perpendicular = normal.cross(movement).cross(normal);

          // The perpendicular movement is properly scaled due to the size of
          // the movement vector (we scaled it above)
          pos += perpendicular;
        }
        else
        {
          // The ray didn't find anything, so just move forward
          pos += movement;
        }

        update_ghost_transform_();
      }

    }

    // ===
    // Grappling-gun physics!
    // ===
    {
      if(input_ref_->primary_attack)
      {
        // Cast a ray in the direction of the player.
        auto ray_rot = xform.getRotation();

        // TODO: Can we do this so willy-nilly?
        ray_rot *= pitch_;

        // Rotate a forward vector by our pithc and yaw to find where the gun is
        // pointing.
        auto forward = btMatrix3x3(ray_rot) * btVector3(0.0f, 0.0f, -1.0f);

        // Do a raycast from the player's head along the forward vector
        auto head_pos = pos + btVector3(0.0f, PLAYER_CAPSULE_HEIGHT / 2, 0.0f);
        auto ray_end = head_pos + forward.normalized() * 1000.0f;

        btCollisionWorld::ClosestRayResultCallback gun_ray(head_pos, ray_end);
        world->rayTest(head_pos, ray_end, gun_ray);

        if(gun_ray.hasHit())
        {
          // Apply some acceleration in the direction of the ray hit
          gun_target_ = ray_end;
          state = Player_State::Flying;
        }
      }
    }

    // ===
    // Apply gravity to the velocity if necessary
    // ===

    // When we are in the air, adjust the velocity change due to gravity.
    if(state == Player_State::Jumping)
    {
      // Adjust the air velocity of the player
      jump_velocity_ += btVector3(0.0f, -9.81f, 0.0f) * dt;
    }
    else if(state == Player_State::Flying)
    {
      // Only when we are flying
      jump_velocity_ += (gun_target_ - pos).normalized() * 20.0f * dt;
    }
    else
    {
      // On the ground, we don't have to move.
      jump_velocity_.setZero();
    }

    // ===
    // Integrate the velocity
    // ===
    if(state != Player_State::Grounded)
    {
      pos += jump_velocity_ * dt;
      update_ghost_transform_();
    }

    // ===
    // Recover the player from any intersection with the map
    //
    // Inspired very heavily by the btKinematicCharacterController.
    // See http://bulletphysics.org/Bullet/BulletFull/btKinematicCharacterController_8cpp_source.html
    // for reference, license, etc.
    // ===

    // Here we must refresh the overlapping paircache as the penetrating
    // movement itself or the previous recovery iteration might have used
    // setWorldTransform and pushed us into an object that is not in the
    // previous cache contents from the last timestep, as will happen if we are
    // pushed into a new AABB overlap. Unhandled this means the next convex
    // sweep gets stuck.

    // Do this by calling the broadphase's setAabb with the moved AABB, this
    // will update the broadphase paircache and the ghostobject's internal
    // paircache at the same time. /BW
    btVector3 minAabb, maxAabb;
    shape_.getAabb(ghost_.getWorldTransform(), minAabb, maxAabb);
    world->getBroadphase()->setAabb(ghost_.getBroadphaseHandle(),
                                    minAabb, maxAabb, world->getDispatcher());

    // Added Notes:

    // The ghost object already knows what it's colliding with but doesn't have
    // any collision manifolds (lists of intersecting points between a pair of
    // objects). Basically, the ghost pair cache and the world pair cache are
    // kept mostly up to date thanks to the btGhostPairCallback set in the
    // world's pair cache above, but the pair objects (and algorithm impl.,
    // which store the collision manifold) are actually different, even if the
    // objects that the pair consists of (or more specifically broadphase
    // proxies) are the same. As a result, we can't pass the world pair cache to
    // do the processing because it will only generate contact points for the
    // pairs of objects in the world. As a result, the contact points (or
    // manifold) will not be accessible from the ghost pair cache, (whose
    // contact points have not been calculated).
    auto ghost_cache = ghost_.getOverlappingPairCache();
    world->getDispatcher()->dispatchAllCollisionPairs(ghost_cache,
                                                      world->getDispatchInfo(),
                                                      world->getDispatcher());

    bool penetration = false;

    for (int i = 0; i < ghost_cache->getNumOverlappingPairs(); i++)
    {
      btManifoldArray manifolds;

      btBroadphasePair* pair = &ghost_cache->getOverlappingPairArray()[i];

      btCollisionObject* obj0 = static_cast<btCollisionObject*>(
              pair->m_pProxy0->m_clientObject);
      btCollisionObject* obj1 = static_cast<btCollisionObject*>(
              pair->m_pProxy1->m_clientObject);

      // Both objects must not have collision response.
      if ((obj0 && !obj0->hasContactResponse()) ||
          (obj1 && !obj1->hasContactResponse()))
      {
        continue;
      }

      if (pair->m_algorithm)
      {
        pair->m_algorithm->getAllContactManifolds(manifolds);
      }

      for (int j = 0; j < manifolds.size(); j++)
      {
        btPersistentManifold* manifold = manifolds[j];
        btScalar sign =
          manifold->getBody0() == &ghost_ ? btScalar(-1.0) : btScalar(1.0);

        for (int p = 0; p < manifold->getNumContacts(); p++)
        {
          const btManifoldPoint& pt = manifold->getContactPoint(p);

          btScalar dist = pt.getDistance();

          if (dist < 0.0)
          {
            // Added notes:

            // This solution of pushing the character away from objects is okay
            // but it is not really practical when the player is standing still
            // The character often glitches around and it's only that measly
            // multiplication by 0.2 that keeps the whole thing somewhat decent
            // and smooth. The smoothing does work against walls though, so this
            // should be considered this codes most important reason for being
            // here. Floor collision, jumping, stepping, etc should all probably
            // be done with a raytrace and a state machine, etc.

            auto normal = pt.m_normalWorldOnB * sign;
            pos += normal * dist;
            penetration = true;
          }
        }
      }
    }

    if(penetration)
    {
      if(state == Player_State::Flying)
      {
        jump_velocity_.setZero();
        state = Player_State::Jumping;
      }
      update_ghost_transform_();
    }
  }

  void Player_Controller::apply_delta_yaw(double yaw)
  {
    auto xform = ghost_.getWorldTransform();
    auto rot = xform.getRotation();
    rot *= btQuaternion(-yaw, 0.0f, 0.0f);
    xform.setRotation(rot);
    ghost_.setWorldTransform(xform);
  }
  void Player_Controller::apply_delta_pitch(double pitch)
  {
    // Don't rotate the ghost object but keep pitch around so we know where to
    // go when the user deploys the grappling gun.
    pitch_ *= btQuaternion(0.0f, -pitch, 0.0f);
  }

  glm::vec3 Player_Controller::get_cam_pos() const
  {
    btTransform trans;
    this->getWorldTransform(trans);
    auto origin = trans.getOrigin();

    // The average person is 7.5 head sizes
    constexpr auto head_size = PLAYER_CAPSULE_HEIGHT / 7.5f;

    // We are at the center of the body, ie 7.5 / 2 or 3.75 heads from the
    // ground, go up 3.50 heads so that we end up in the middle of the topmost
    // head, which is obviously just the players head! (3.75 + 3.50 = 7.25)
    return {origin.x(), origin.y() + 3.50f * head_size, origin.z()};
  }
}

#include "player.h"

#include "common/log.h"

// All of this is in meters
// My own height, slightly smaller radius
#define PLAYER_HEIGHT 1.63f
#define PLAYER_RADIUS 0.24f

// Height of things the player can step over easily, this is the size of the ray
// and is part of the player's total height.
#define PLAYER_SHOE_SIZE 0.15f

#define PLAYER_CAPSULE_HEIGHT \
  (PLAYER_HEIGHT - 2 * PLAYER_RADIUS - PLAYER_SHOE_SIZE)

// Yer average Slavs' Squating
#define CROUCHED_HEIGHT 1.1f
#define CROUCHED_CAPSULE_HEIGHT \
  (CROUCHED_HEIGHT - 2 * PLAYER_RADIUS - PLAYER_SHOE_SIZE)

// Make sure these values aren't stupid
static_assert(PLAYER_CAPSULE_HEIGHT > 0.0f,
              "Standing player must have height");
static_assert(CROUCHED_CAPSULE_HEIGHT > 0.0f,
              "Crouched player must have height");

#define PLAYER_SPEED 1.0f
#define CROUCHED_SPEED 0.5f

#define PLAYER_STEP_RATE 0.5f;
#define CROUCHED_STEP_RATE 1.0f;

// 78 kg mass person
#define PLAYER_MASS 78.0f

namespace redc
{
  Player_Controller::Player_Controller()
    : inited_(false), ghost_(), shape_(PLAYER_RADIUS, PLAYER_CAPSULE_HEIGHT),
      crouch_shape_(PLAYER_RADIUS, CROUCHED_CAPSULE_HEIGHT),
      jump_velocity_()
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
    trans.setOrigin(btVector3(0.0f, 2.0f, 0.0f));
    ghost_.setWorldTransform(trans);

    // Shape
    ghost_.setCollisionShape(&shape_);
    set_normal_props();

    // Flags
    ghost_.setCollisionFlags(ghost_.getCollisionFlags() |
                             btCollisionObject::CF_KINEMATIC_OBJECT);
    ghost_.setActivationState(DISABLE_DEACTIVATION);


    // Set the pitch to an identity rotation.
    pitch_.setEuler(0.0f, 0.0f, 0.0f);
  }

  void Player_Controller::set_normal_props()
  {
    player_props_.total_height = PLAYER_HEIGHT;
    player_props_.capsule_height = PLAYER_CAPSULE_HEIGHT;

    player_props_.radius = PLAYER_RADIUS;
    player_props_.shoe_size = PLAYER_SHOE_SIZE;

    player_props_.mass = PLAYER_MASS;
    player_props_.speed = PLAYER_SPEED;

    player_props_.step_rate = PLAYER_STEP_RATE;

    player_props_.is_crouched = false;
  }
  void Player_Controller::set_crouch_props()
  {
    player_props_.total_height = CROUCHED_HEIGHT;
    player_props_.capsule_height = CROUCHED_CAPSULE_HEIGHT;

    player_props_.radius = PLAYER_RADIUS;
    player_props_.shoe_size = PLAYER_SHOE_SIZE;

    player_props_.mass = PLAYER_MASS;
    player_props_.speed = CROUCHED_SPEED;

    player_props_.step_rate = CROUCHED_STEP_RATE;

    player_props_.is_crouched = true;
  }

  // They are expected to add the action to the dynamics world and then this
  // will be called, then we init then we start
  void Player_Controller::updateAction(btCollisionWorld* world, btScalar dt)
  {
    if(!inited_)
    {
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

    Player_Properties const& props = player_props_;

    // ===
    // Ray tracing floor-collision handling
    // ===
    {
      // First trace a ray from the player to the near-ground. If we hit
      // something we should consider the player on the ground, that means we no
      // longer have to apply gravity, for example.

      // Start at the bottom of the player's capsule and continue till the of
      // the player's "shoes."

      auto start_pos = pos;
      start_pos.setY(pos.getY() - props.capsule_height / 2.0f - props.radius);

      auto end_pos = pos;
      end_pos.setY(start_pos.getY() - props.shoe_size);

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

      // If the player isn't grounded, we should not consider walking
      if(state != Player_State::Grounded)
      {
        // Reset the walk timer
        walk_timer_.reset();
      }

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

      // Crouch is down and we are not currently crouching.
      if(!input_ref_->crouch && props.is_crouched)
      {
        ghost_.setCollisionShape(&shape_);
        set_normal_props();
      }
      else if(input_ref_->crouch && !props.is_crouched)
      {
        ghost_.setCollisionShape(&crouch_shape_);
        set_crouch_props();
      }

      if(input_ref_->jump && state == Player_State::Grounded)
      {
        // If we are on the ground, apply an impulse to the player upward (ie
        // change the velocity).
        jump_velocity_ = btVector3(0.0f, 2.943f, 0.0f);
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
        movement.normalize() *= props.speed * dt;

        auto walk_time = walk_timer_.has_been<std::chrono::milliseconds>();
        auto walk_s = walk_time.count() / 1000.0f;
        if(walk_s > props.step_rate)
        {
          Player_Event event;
          event.type = Player_Event::Footstep;
          events_.push_outgoing_event(event);

          walk_timer_.reset();
        }
        // Cast a ray from the character's position to the position plus the
        // movement vector. This is obviously where the player is trying to go.
        auto end_pt = pos + movement.normalized() * props.radius * 4.5f;
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
        // TODO: Inspect this use of total height, it looks suspicious
        auto head_pos = pos + btVector3(0.0f, props.total_height / 2, 0.0f);
        auto ray_end = head_pos + forward.normalized() * 1000.0f;

        btCollisionWorld::ClosestRayResultCallback gun_ray(head_pos, ray_end);
        world->rayTest(head_pos, ray_end, gun_ray);

        if(gun_ray.hasHit())
        {
          // Apply some acceleration in the direction of the ray hit
          gun_target_ = gun_ray.m_hitPointWorld;
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
      jump_velocity_ += btVector3(0.0f, -9.81f, 0.0f) * dt;
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
    ghost_.getCollisionShape()->getAabb(ghost_.getWorldTransform(),
                                        minAabb, maxAabb);
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

  glm::vec3 Player_Controller::get_player_pos() const
  {
    btTransform trans;
    this->getWorldTransform(trans);

    const btVector3 origin = trans.getOrigin();

    // Find the y-value of the floor beneath the player, given the center point
    // of the capsule.
    const float floor_y = origin.getY() - (player_props_.capsule_height / 2.0f +
                                           player_props_.radius +
                                           player_props_.shoe_size);

    // Return the point the player is standing on
    return {origin.getX(), floor_y, origin.getZ()};
  }
  glm::vec3 Player_Controller::get_cam_pos() const
  {
    const glm::vec3 player_pos = get_player_pos();

    return {player_pos.x, player_pos.y + player_props_.total_height,
            player_pos.z};
  }
}

#include "player.h"

#include "common/log.h"

#define PLAYER_CAPSULE_HEIGHT 1.8f
#define PLAYER_CAPSULE_RADIUS 0.25f

// 75 kg mass person
#define PLAYER_MASS 75.0f

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
      trans.setOrigin(btVector3(0.0f, 1.5f, 0.0f));
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
        state = Player_State::Jumping;
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
        local_dpos.setX(-1.0f);
      }
      if(input_ref_->backward)
      {
        local_dpos.setX(+1.0f);
      }
      if(input_ref_->strafe_left)
      {
        local_dpos.setZ(+1.0f);
      }
      if(input_ref_->strafe_right)
      {
        local_dpos.setZ(-1.0f);
      }

      if(input_ref_->jump && state == Player_State::Grounded)
      {
        jump_velocity_ = btVector3(0.0f, 4.0f, 0.0f);
        state = Player_State::Jumping;
      }

      auto active_normal = last_normal_;
      if(state == Player_State::Jumping)
      {
        active_normal.setZero();
        active_normal.setY(1.0f);
      }

      if(FLT_EPSILON < btFabs(local_dpos.length()))
      {
        auto movement = btMatrix3x3(xform.getRotation()) * local_dpos;
        pos += movement.cross(active_normal) * btScalar(0.1f);
      }

      update_ghost_transform_();
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
    else
    {
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

      bool handled = false;
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
            pos += pt.m_normalWorldOnB * sign * dist * btScalar(0.5);
            penetration = true;
          }
        }
      }
    }

    if(penetration) update_ghost_transform_();
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
    // We don't care about pitch as of now from a physics standpoint
    /*
    auto xform = ghost_.getWorldTransform();
    auto rot = xform.getRotation();
    rot *= btQuaternion(0.0f, pitch, 0.0f);
    ghost_.setWorldTransform(xform);
    */
  }
}

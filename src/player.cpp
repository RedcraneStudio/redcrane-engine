#include "player.h"

#include "common/log.h"

#define PLAYER_CAPSULE_HEIGHT 1.8f
#define PLAYER_CAPSULE_RADIUS 0.25f

namespace redc
{
  Player_Controller::Player_Controller() : inited_(false),
                                           ghost_(),
                                           shape_(PLAYER_CAPSULE_RADIUS,
                                                  PLAYER_CAPSULE_HEIGHT) {}

  // They are expected to add the action to the dynamics world and then this
  // will be called, then we init then we start
  void Player_Controller::updateAction(btCollisionWorld* world, btScalar dt)
  {
    if(!inited_)
    {
      btTransform trans;
      trans.setIdentity();
      trans.setOrigin(btVector3(0.0f, .5f, 0.0f));
      ghost_.setWorldTransform(trans);
      ghost_.setCollisionShape(&shape_);
      world->addCollisionObject(&ghost_);
      ghost_.setCollisionFlags(ghost_.getCollisionFlags() |
                               btCollisionObject::CF_KINEMATIC_OBJECT);
      ghost_.setActivationState(DISABLE_DEACTIVATION);

      world->getPairCache()->setInternalGhostPairCallback(&ghost_cb_);
      inited_ = true;
      return;
    }

    auto xform = ghost_.getWorldTransform();
    {
      auto orig = xform.getOrigin();

      // Apply some sort of gravity
      orig.setY(orig.getY() - .05f);
      xform.setOrigin(orig);
    }
    ghost_.setWorldTransform(xform);

    // ===
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
    // proxies) are the same. As a result, we can't pass the world pair cache
    // to do the processing because it will only generate contact points for
    // the pairs of objects in the world. As a result, the contact points (or
    // manifold) will not be accessible from the ghost pair cache, (whose
    // contact points have not been calculated).
    auto ghost_cache = ghost_.getOverlappingPairCache();
    world->getDispatcher()->dispatchAllCollisionPairs(ghost_cache,
                                                      world->getDispatchInfo(),
                                                      world->getDispatcher());

    bool penetration = false;

    xform = ghost_.getWorldTransform();
    auto cur_pos = xform.getOrigin();

    for (int i = 0; i < ghost_cache->getNumOverlappingPairs(); i++)
    {
      btManifoldArray manifolds;

      btBroadphasePair* pair = &ghost_cache->getOverlappingPairArray()[i];

      btCollisionObject* obj0 = static_cast<btCollisionObject*>(
              pair->m_pProxy0->m_clientObject);
      btCollisionObject* obj1 = static_cast<btCollisionObject*>(
              pair->m_pProxy1->m_clientObject);

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
        btScalar directionSign =
          manifold->getBody0() == &ghost_ ? btScalar(-1.0) : btScalar(1.0);

        for (int p = 0; p < manifold->getNumContacts(); p++)
        {
          const btManifoldPoint& pt = manifold->getContactPoint(p);

          btScalar dist = pt.getDistance();

          if (dist < 0.0)
          {
            cur_pos += pt.m_normalWorldOnB * directionSign * dist * btScalar(0.2);
            penetration = true;
          }
        }
      }
    }
    xform.setOrigin(cur_pos);

    btVector3 local_dpos(0.0f, 0.0f, 0.0f);

    if(input_ref_->forward)
    {
      local_dpos.setZ(-.1f);
    }
    if(input_ref_->backward)
    {
      local_dpos.setZ(+.1f);
    }
    if(input_ref_->strafe_left)
    {
      local_dpos.setX(-.1f);
    }
    if(input_ref_->strafe_right)
    {
      local_dpos.setX(+.1f);
    }
    if(input_ref_->primary_attack)
    {
      local_dpos.setY(+.1f);
    }
    if(input_ref_->secondary_attack)
    {
      local_dpos.setY(-.1f);
    }

    auto orig = xform.getOrigin();
    orig += btMatrix3x3(xform.getRotation()) * local_dpos;
    xform.setOrigin(orig);

    ghost_.setWorldTransform(xform);
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

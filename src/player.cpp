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

    auto cur_pos = xform.getOrigin();
    auto end_pos = cur_pos;

    // The player's height ought to be enough
    end_pos.setY(cur_pos.getY() - PLAYER_CAPSULE_HEIGHT / 2.0f);

    // Perform a raytest from the character to the ground and lock it there.
    btCollisionWorld::ClosestRayResultCallback ray_cb(cur_pos, end_pos);
    world->rayTest(cur_pos, end_pos, ray_cb);
    if(ray_cb.hasHit())
    {
      auto end = ray_cb.m_hitPointWorld;

      // Put it right on top!
      end.setY(end.getY() + PLAYER_CAPSULE_HEIGHT / 2.0f);
      xform.setOrigin(end);
    }

#if 0
    // Recover from intersection
    btVector3 min_aabb, max_aabb;
    shape_.getAabb(ghost_.getWorldTransform(), min_aabb, max_aabb);
    world->getBroadphase()->setAabb(ghost_.getBroadphaseHandle(), min_aabb,
                                    max_aabb, world->getDispatcher());
    bool penetration = false;
    world->getDispatcher()->dispatchAllCollisionPairs(
            ghost_.getOverlappingPairCache(),
            world->getDispatchInfo(),
            world->getDispatcher());

    btScalar maxPen = btScalar(0.0);
#endif

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

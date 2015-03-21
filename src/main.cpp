/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "scoped_drop.hpp"
#include "irrlicht.h"

int main(int argc, char** argv)
{
  namespace irrc = irr::core;

  using irr::s32;
  using irr::u32;
  using irrc::rect;

  using irrc::vector3df;
  using irr::video::SColor;

  auto device = irr::createDevice(irr::video::EDT_OPENGL,
                                  irrc::dimension2d<u32>(1000, 1000), 16,
                                  false, false, false, 0);
  auto scoped_device_drop = survive::make_scoped_drop(device);

  device->setWindowCaption(L"Survival Games");

  auto video_driver = device->getVideoDriver();
  auto scene_manager = device->getSceneManager();
  auto gui_env = device->getGUIEnvironment();

  gui_env->addStaticText(L"Hello world, brah!", rect<s32>(10, 10, 200, 100),
                         true);

  auto mesh = scene_manager->getMesh("FitFox00.obj");
  if(!mesh) return EXIT_FAILURE;

  auto node = scene_manager->addAnimatedMeshSceneNode(mesh);
  if(node)
  {
    node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  }

  scene_manager->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));
  while(device->run())
  {
    video_driver->beginScene(true, true, SColor(255, 100, 101, 140));

    scene_manager->drawAll();
    gui_env->drawAll();

    video_driver->endScene();
  }

  // The device is dropped here by our scoped helper constructed above.
  return EXIT_SUCCESS;
}

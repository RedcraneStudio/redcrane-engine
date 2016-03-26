/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * The engine's C interface for the engine to use
 */
#include <cstdint>
#include <memory>

#include <boost/variant.hpp>

#include "minilua.h"

#include "common/id_map.hpp"

#include "use/mesh_cache.h"

#include "gfx/idriver.h"
#include "gfx/camera.h"
#include "gfx/mesh_chunk.h"

#include "fps/camera_controller.h"

#include "sdl_helper.h"

extern "C"
{
  #include "redcrane_decl.h"
}

namespace redc
{
  struct Engine
  {
    redc::SDL_Init_Lock sdl_raii;

    std::unique_ptr<gfx::IDriver> driver;
    std::unique_ptr<gfx::Mesh_Cache> mesh_cache;

    bool running = true;

    // TODO: Maybe keep track of every scene so lua doesn't have to deal with it
    // We should reserve some amount of memory for each scene so that we can
    // pass around pointers and no they won't suddenly become invalid.
  };

  struct Mesh_Object
  {
    gfx::Mesh_Chunk chunk;
  };

  struct Cam_Object
  {
    Cam_Object(gfx::Camera const& c) : cam(c), control() {}

    gfx::Camera cam;
    fps::Camera_Controller control;
  };

  struct Object
  {
    // This must match the order in the boost variant!!!
    enum Obj_Type : unsigned int
    {
      Mesh = 0,
      Cam = 1
    };

    boost::variant<Mesh_Object, Cam_Object> obj;

    // Non-owned parent, they should all be in the vector.
    Object* parent;
  };

  struct Scene
  {
    // This is an unordered map that also keeps an active camera available to
    // us.
    Engine* engine;

    // Should always be a Obj_Type::Cam!! Can we enforce that with the type
    // system?
    obj_id active_camera;

    ID_Gen<obj_id> index_gen;
    std::array<Object, std::numeric_limits<obj_id>::max()> objs;
  };
}

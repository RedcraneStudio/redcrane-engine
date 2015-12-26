/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "boat.h"
#include "../common/log.h"
namespace game
{
  Boat_Descs build_default_descs(gfx::Mesh_Cache& mc) noexcept
  {
    Boat_Descs ret;

    ret.hull_descs.push_back(
      build_hull_desc(mc, "Sailboat Hull", "obj/hulls/sailhull.obj",
                      {{0.0f, +0.30366f, +0.07065f},
                       {0.0f, -0.83811f, +0.26013f},
                       {0.0f, +0.94225f, +0.23586f}})
    );

    ret.sail_descs.push_back(
      build_sail_desc(mc, "Normal Sail", "obj/sails/sail.obj")
    );

    ret.rudder_descs.push_back(
      build_rudder_desc(mc, "Normal Rudder", "obj/rudders/rudder.obj")
    );

    ret.gun_descs.push_back(
      build_gun_desc(mc, "Cannon", "obj/guns/cannon.obj")
    );
    ret.gun_descs.push_back(
      build_gun_desc(mc, "Gattling Gun", "obj/guns/gattlinggun.obj")
    );

    return ret;
  }
  void log_boat_descs(Boat_Descs& boat_descs) noexcept
  {
    log_i("Finished loading the following hulls:");
    for(std::size_t i = 0; i < boat_descs.hull_descs.size(); ++i)
    { log_i("  - (%) %", i, boat_descs.hull_descs[i].name); }
    flush_log_full();

    log_i("Finished loading the following sails:");
    for(std::size_t i = 0; i < boat_descs.sail_descs.size(); ++i)
    { log_i("  - (%) %", i, boat_descs.sail_descs[i].name); }
    flush_log_full();

    log_i("Finished loading the following rudders:");
    for(std::size_t i = 0; i < boat_descs.rudder_descs.size(); ++i)
    { log_i("  - (%) %", i, boat_descs.rudder_descs[i].name); }
    flush_log_full();

    log_i("Finished loading the following guns:");
    for(std::size_t i = 0; i < boat_descs.gun_descs.size(); ++i)
    { log_i("  - (%) %", i, boat_descs.gun_descs[i].name); }
    flush_log_full();
  }
}

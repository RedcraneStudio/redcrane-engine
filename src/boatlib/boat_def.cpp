/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "boat.h"
#include "../common/log.h"
namespace redc
{
  Boat_Descs build_default_descs(gfx::IDriver& d, gfx::Mesh_Cache& mc)
  {
    Boat_Descs ret;

    ret.hull_descs.push_back(
      build_hull_desc(d, mc, "Sailboat Hull", "hulls/sailhull",
                      {{0.0f, +0.0000f, 0.01013f}, // Sail
                       {0.0f, +0.0f, +0.0f}, // Rudder
                       {0.0f, +0.0f, 0.0f}}) // Gun
    );

    ret.sail_descs.push_back(
      build_object_desc<Sail_Desc>(d, mc, "Normal Sail", "sails/sail")
    );

    ret.rudder_descs.push_back(
      build_object_desc<Rudder_Desc>(d, mc, "Normal Rudder", "rudders/rudder")
    );

    ret.gun_descs.push_back(
      build_object_desc<Gun_Desc>(d, mc, "Cannon", "guns/cannon")
    );
    ret.gun_descs.push_back(
      build_object_desc<Gun_Desc>(d, mc, "Gattling Gun", "guns/gattlinggun")
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

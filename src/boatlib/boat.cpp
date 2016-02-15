/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "boat.h"
namespace redc
{
  Hull_Desc build_hull_desc(gfx::IDriver& d, gfx::Mesh_Cache& m_cache,
                            std::string name, std::string model_name,
                            Hull_Attachment_Def attachments) noexcept
  {
    Hull_Desc ret;

    ret.name = name;
    ret.mesh = load_chunk(d, m_cache, model_name);

    ret.attachments = attachments;

    return ret;
  }

  Boat_Render_Config build_boat_render_config(Boat_Config& boat) noexcept
  {
    Boat_Render_Config ret;

    ret.attachments = boat.hull->attachments;

    ret.hull = copy_mesh_chunk_share_mesh(boat.hull->mesh);
    ret.sail = copy_mesh_chunk_share_mesh(boat.sail->mesh);
    ret.rudder = copy_mesh_chunk_share_mesh(boat.rudder->mesh);
    ret.gun = copy_mesh_chunk_share_mesh(boat.gun->mesh);

    ret.model = glm::mat4(1.0f);

    return ret;
  }
}

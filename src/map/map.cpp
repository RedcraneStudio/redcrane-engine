/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"

#include "../gfx/camera.h"
#include "../gfx/idriver.h"

#include <glm/gtc/matrix_transform.hpp>
namespace game
{
  Structure_Instance::Structure_Instance(Structure& s, pos_t p) noexcept
    : position(p), s_type_(&s){}

  void Structure_Instance::set_structure_type(Structure const& s) noexcept
  {
    s_type_ = &s;
  }
  Structure const& Structure_Instance::structure() noexcept
  {
    return *s_type_;
  }

  pos_t unproject_mouse_coordinates(gfx::IDriver& d, gfx::Camera& cam,
                                    glm::mat4 const& mod, Vec<int> v) noexcept
  {
    // Read pixel automatically inverts the y.
    auto depth = d.read_pixel(gfx::Framebuffer::Depth, v);

    // @ Refactor: Find out how to abstract the y-axis mouse-coordinate
    // inversion.
    glm::vec3 win((float) v.x, (float) d.window_extents().y - v.y, depth);

    glm::vec4 viewport(0, 0, d.window_extents().x, d.window_extents().y);

    auto obj_coord = glm::unProject(win, camera_view_matrix(cam) * mod,
                                    camera_proj_matrix(cam), viewport);

    return {obj_coord.x, obj_coord.z};
  }
}

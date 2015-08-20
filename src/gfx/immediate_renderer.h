/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/aabb.h"
#include "camera.h"
#include "idriver.h"
namespace game { namespace gfx
{
  // It's stupid to have two immediate-like renderers that just have different
  // functionally but implement it largely the same way.
  struct Immediate_Renderer
  {
    Immediate_Renderer(IDriver& d) noexcept;

    void set_draw_color(Color const&) noexcept;

    void draw_aabb(AABB const& aabb) noexcept;
    void draw_line(glm::vec3 const& pt1, glm::vec3 const& pt2) noexcept;

    void reset() noexcept;
    void render(Camera const& cam) noexcept;
  private:
    IDriver* d_;

    Mesh::buf_t pos_buf_;
    std::size_t pos_pos_;
    std::unique_ptr<Shader> shader_;

    std::unique_ptr<Mesh> mesh_;

    Color cur_dif_;

    std::size_t buf_size_;
  };
} }

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "envmap.h"
#include "../use/texture.h"
namespace redc { namespace effects
{
  void Envmap_Effect::init(gfx::IDriver& driver,
                           po::variables_map const&) noexcept
  {
    // Load environment map
    envmap_ = gfx::load_cubemap(driver, "tex/envmap/front.png",
                                "tex/envmap/back.png",
                                "tex/envmap/right.png",
                                "tex/envmap/left.png",
                                "tex/envmap/up.png",
                                "tex/envmap/down.png");

    std::vector<float> cube_data =
    {
      -1.0f, +1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, +1.0f, -1.0f,
      -1.0f, +1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f, +1.0f, -1.0f,
      -1.0f, +1.0f, -1.0f,
      -1.0f, +1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f,  1.0f,
       1.0f, +1.0f,  1.0f,
       1.0f, +1.0f,  1.0f,
       1.0f, +1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f, +1.0f,  1.0f,
       1.0f, +1.0f,  1.0f,
       1.0f, +1.0f,  1.0f,
       1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

      -1.0f,  1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f,  1.0f
    };

    mesh_ = driver.make_mesh_repr();
    auto data_buf =
      mesh_->allocate_buffer(cube_data.size() * sizeof(float),
                             Usage_Hint::Draw, Upload_Hint::Static);
    mesh_->format_buffer(data_buf, 0, 3, Buffer_Format::Float, 0,
                               0);
    mesh_->enable_vertex_attrib(0);
    mesh_->set_primitive_type(Primitive_Type::Triangle);
    mesh_->buffer_data(data_buf, 0, sizeof(float) * cube_data.size(),
                       &cube_data[0]);

    shader_ = driver.make_shader_repr();
    shader_->load_vertex_part("shader/envmap/vs.glsl");
    shader_->load_fragment_part("shader/envmap/fs.glsl");

    view_loc_ = shader_->get_location("view");
    proj_loc_ = shader_->get_location("proj");
    shader_->set_integer(shader_->get_location("envmap"), 0);

    elements_ = cube_data.size() / 3;
  }
  void Envmap_Effect::render(gfx::IDriver& driver,
                             gfx::Camera const& cam) noexcept
  {
    // Render the environment map
    driver.use_shader(*shader_);

    // Bind the texture
    driver.bind_texture(*envmap_, 0);

    glm::mat4 env_camera_mat = camera_view_matrix(cam);
    // Zero out the translation
    env_camera_mat[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    shader_->set_matrix(view_loc_, env_camera_mat);
    shader_->set_matrix(proj_loc_, camera_proj_matrix(cam));

    driver.write_depth(false);
    driver.depth_test(false);
    mesh_->draw_arrays(0, elements_);
    driver.write_depth(true);
    driver.depth_test(true);
  }
} }

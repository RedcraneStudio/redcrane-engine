/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "envmap.h"
#include "../use/texture.h"
namespace redc { namespace effects
{
  void Envmap_Effect::init(gfx::IDriver& driver) noexcept
  {
    // Load environment map
    envmap_ = gfx::load_cubemap(driver, "../assets/tex/envmap/front.png",
                                "../assets/tex/envmap/back.png",
                                "../assets/tex/envmap/right.png",
                                "../assets/tex/envmap/left.png",
                                "../assets/tex/envmap/up.png",
                                "../assets/tex/envmap/down.png");

    envmap_->set_mag_filter(gfx::Texture_Filter::Linear);
    envmap_->set_min_filter(gfx::Texture_Filter::Linear);

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

    pos_buf_ = driver.make_buffer_repr();
    pos_buf_->allocate(gfx::Buffer_Target::Array,
                       cube_data.size() * sizeof(float),
                       &cube_data[0], gfx::Usage_Hint::Draw,
                       gfx::Upload_Hint::Static);


    gfx::Attrib_Bind bind = 0;
    mesh_->format_buffer(*pos_buf_, bind, gfx::Attrib_Type::Vec3,
                         gfx::Data_Type::Float, 0, 0);
    mesh_->enable_attrib_bind(bind);

    mesh_->set_primitive_type(gfx::Primitive_Type::Triangles);

    shader_ = driver.make_shader_repr();
    load_vertex_file(*shader_, "../assets/shader/envmap/vs.glsl");
    load_fragment_file(*shader_, "../assets/shader/envmap/fs.glsl");
    shader_->link();

    shader_->set_var_tag(gfx::tags::view_tag, "view");
    shader_->set_var_tag(gfx::tags::proj_tag, "proj");

    shader_->set_var_tag(gfx::tags::envmap_tag, "envmap");
    shader_->set_integer(gfx::tags::envmap_tag, 0);

    elements_ = cube_data.size() / 3;
  }
  void Envmap_Effect::render(gfx::IDriver& driver,
                             gfx::Camera const& cam) noexcept
  {
    // Render the environment map
    driver.use_shader(*shader_);

    // Bind the texture
    driver.active_texture(0);
    driver.bind_texture(*envmap_, gfx::Texture_Target::Cube_Map);

    glm::mat4 env_camera_mat = camera_view_matrix(cam);
    // Zero out the translation
    env_camera_mat[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    shader_->set_mat4(gfx::tags::view_tag, env_camera_mat);
    shader_->set_mat4(gfx::tags::proj_tag, camera_proj_matrix(cam));

    driver.write_depth(false);
    driver.depth_test(false);
    mesh_->draw_arrays(0, elements_);
    driver.write_depth(true);
    driver.depth_test(true);
  }
} }

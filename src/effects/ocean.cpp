/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "ocean.h"
#include "../use/texture.h"

#include "../water/grid.h"

namespace redc { namespace effects
{
  void Ocean_Effect::init(gfx::IDriver& driver,
                          po::variables_map const&) noexcept
  {
    // Make a grid and upload it
    auto water_grid = water::gen_grid(200);
    water_base_ = {{0.0f, 1.0f, 0.0f}, 0.0f};

    elements_ = water_grid.size();

    grid_mesh_ = driver.make_mesh_repr();
    auto uv_buf =
      grid_mesh_->allocate_buffer(sizeof(float) * 2 * water_grid.size(),
                                 redc::Usage_Hint::Draw,
                                 redc::Upload_Hint::Static);
    grid_mesh_->format_buffer(uv_buf, 0, 2, Buffer_Format::Float, 0, 0);
    grid_mesh_->enable_vertex_attrib(0);
    grid_mesh_->buffer_data(uv_buf, 0, sizeof(float) * 2 * water_grid.size(),
                           &water_grid[0]);
    grid_mesh_->set_primitive_type(Primitive_Type::Triangle);

    // Initialize the shader
    shader_ = driver.make_shader_repr();
    shader_->load_vertex_part("shader/water/vs.glsl");
    shader_->load_fragment_part("shader/water/fs.glsl");

    auto plane_loc = shader_->get_location("plane");
    shader_->set_vec4(plane_loc, plane_as_vec4(water_base_));

    time_loc_ = shader_->get_location("time");
    shader_->set_float(time_loc_, 0.0f);

    Ocean_Gen params;
    params.octaves = 5;
    params.amplitude = 0.2f;
    params.frequency = 0.5f;
    params.persistence = .5f;
    params.lacunarity = .6f;
    set_ocean_gen_parameters(params);

    projector_loc_ = shader_->get_location("projector");
    shader_->set_view_name("view");
    shader_->set_projection_name("proj");

    shader_->set_matrix(projector_loc_, glm::mat4(1.0f));
    shader_->set_view(glm::mat4(1.0f));
    shader_->set_projection(glm::mat4(1.0f));

    cam_pos_loc_ = shader_->get_location("camera_pos");
    light_dir_loc_ = shader_->get_location("light_dir");

    auto water_envmap_loc = shader_->get_location("envmap");
    shader_->set_integer(water_envmap_loc, 0);

    start_ = std::chrono::high_resolution_clock::now();
  }
  void Ocean_Effect::render(gfx::IDriver& driver,
                            gfx::Camera const& cam) noexcept
  {
    // Render water

    driver.use_shader(*shader_);
    use_camera(driver, cam);
    shader_->set_vec3(cam_pos_loc_, cam.look_at.eye);

    // In case they have changed in the meantime
    update_ocean_gen_params();

    shader_->set_vec3(light_dir_loc_,
      glm::normalize(glm::vec3(5.0f, 5.0f, -6.0f)));

    auto intersections = water::find_visible(cam, water_base_.dist, max_disp_);
    if(intersections.size())
    {
      auto projector = water::build_projector(cam, water_base_, max_disp_);
      auto range = water::build_min_max_mat(intersections, projector, water_base_);
      projector = projector * range;

      shader_->set_matrix(projector_loc_, projector);

      namespace chrono = std::chrono;

      // TODO: Utilize the dt given to the render task instead of rolling our
      // TODO: own!
      // current time
      auto now = chrono::high_resolution_clock::now();
      // microseconds since start
      auto us = chrono::duration_cast<chrono::microseconds>(now - start_);
      // time in seconds since start
      float time_s = us.count() / 10000.0f;

      shader_->set_float(time_loc_, time_s);

      grid_mesh_->draw_arrays(0, elements_);
    }
  }

  void Ocean_Effect::set_ocean_gen_parameters(Ocean_Gen const& gen) noexcept
  {
    gen_params_ = gen;
    needs_gen_params_update_ = true;
  }

  void Ocean_Effect::update_ocean_gen_params() noexcept
  {
    // Update the uniforms in the shader.
    if(needs_gen_params_update_)
    {
      needs_gen_params_update_ = false;

      shader_->set_integer(shader_->get_location("octaves_in"),
                           gen_params_.octaves);
      shader_->set_float(shader_->get_location("amplitude_in"),
                         gen_params_.amplitude);
      shader_->set_float(shader_->get_location("frequency_in"),
                         gen_params_.frequency);
      shader_->set_float(shader_->get_location("persistence_in"),
                         gen_params_.persistence);
      shader_->set_float(shader_->get_location("lacunarity_in"),
                         gen_params_.lacunarity);

      // TODO: Base this off the above parameters.
      max_disp_ = 1.0f;
    }
  }
} }

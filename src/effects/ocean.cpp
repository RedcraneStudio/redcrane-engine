/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "ocean.h"
#include "../use/texture.h"

#include "../gfx/support/proj_grid.h"

#include "../gfx/shader.h"

namespace redc { namespace effects
{
  void Ocean_Effect::init(gfx::IDriver& driver) noexcept
  {
    // Make a grid and upload it
    auto water_grid = proj_grid::gen_grid(200);
    water_base_ = {{0.0f, 1.0f, 0.0f}, 0.0f};

    elements_ = water_grid.size();

    grid_mesh_ = driver.make_mesh_repr();
    auto uv_buf =
      grid_mesh_->allocate_buffer(sizeof(float) * 2 * water_grid.size(),
                                 redc::Usage_Hint::Draw,
                                 redc::Upload_Hint::Static);
    grid_mesh_->format_buffer(uv_buf, 0, 2, Data_Type::Float, 0, 0);
    grid_mesh_->enable_vertex_attrib(0);
    grid_mesh_->buffer_data(uv_buf, 0, sizeof(float) * 2 * water_grid.size(),
                           &water_grid[0]);
    grid_mesh_->set_primitive_type(Primitive_Type::Triangle);

    // Initialize the shader
    shader_ = driver.make_shader_repr();
    gfx::load_vertex_file(*shader_, "shader/water/vs.glsl");
    gfx::load_fragment_file(*shader_, "shader/water/fs.glsl");
    shader_->link();

    Ocean_Gen params;
    params.octaves = 5;
    params.amplitude = 0.2f;
    params.frequency = 0.5f;
    params.persistence = .5f;
    params.lacunarity = .6f;
    set_ocean_gen_parameters(params);

    shader_->set_var_tag(gfx::tags::view_tag, "view");
    shader_->set_var_tag(gfx::tags::proj_tag, "proj");
    shader_->set_var_tag(gfx::tags::envmap_tag, "envmap");
    shader_->tag_var("projector");
    shader_->tag_var("camera_pos");
    shader_->tag_var("light_dir");
    shader_->tag_var("plane");
    shader_->tag_var("time");

    shader_->set_mat4(gfx::tags::proj_tag, glm::mat4(1.0f));
    shader_->set_mat4(gfx::tags::proj_tag, glm::mat4(1.0f));
    shader_->set_mat4(gfx::tags::proj_tag, glm::mat4(1.0f));
    shader_->set_integer(gfx::tags::envmap_tag, 0);

    shader_->set_vec4("plane", plane_as_vec4(water_base_));
    shader_->set_float("time", 0.0f);

    start_ = std::chrono::high_resolution_clock::now();
  }
  void Ocean_Effect::render(gfx::IDriver& driver,
                            gfx::Camera const& cam) noexcept
  {
    // Render water

    driver.use_shader(*shader_);
    use_camera(driver, cam);
    shader_->set_vec3("camera_pos", cam.look_at.eye);

    // In case they have changed in the meantime
    update_ocean_gen_params();

    shader_->set_vec3("light_dir",
      glm::normalize(glm::vec3(5.0f, 5.0f, -6.0f)));

    auto intersections = proj_grid::find_visible(cam, water_base_.dist, max_disp_);
    if(intersections.size())
    {
      auto projector = proj_grid::build_projector(cam, water_base_, max_disp_);
      auto range = proj_grid::build_min_max_mat(intersections, projector, water_base_);
      projector = projector * range;

      shader_->set_mat4(gfx::tags::proj_tag, projector);

      namespace chrono = std::chrono;

      // TODO: Utilize the dt given to the render task instead of rolling our
      // TODO: own!
      // current time
      auto now = chrono::high_resolution_clock::now();
      // microseconds since start
      auto us = chrono::duration_cast<chrono::microseconds>(now - start_);
      // time in seconds since start
      float time_s = us.count() / 1000000.0f;

      // This a sorry excuse for a hack
      last_time_used_ = time_s;

      shader_->set_float("time", time_s);

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

      shader_->set_integer("octaves_in", gen_params_.octaves);
      shader_->set_float("amplitude_in", gen_params_.amplitude);
      shader_->set_float("frequency_in", gen_params_.frequency);
      shader_->set_float("persistence_in", gen_params_.persistence);
      shader_->set_float("lacunarity_in", gen_params_.lacunarity);

      // TODO: Base this off the above parameters.
      max_disp_ = 1.0f;
    }
  }
} }

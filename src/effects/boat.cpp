/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "boat.h"
#include <glm/gtx/transform.hpp>
#include "../gfx/idriver.h"
#include "../common/log.h"
namespace redc { namespace effects
{
  void render_boat(gfx::Shader& shader,
                   Boat_Render_Config const& boat_render) noexcept
  {
    shader.set_model(boat_render.model);
    gfx::render_chunk(boat_render.hull);

    shader.set_model(glm::translate(boat_render.model,
                                    boat_render.attachments.sail));
    gfx::render_chunk(boat_render.sail);
    shader.set_model(glm::translate(boat_render.model,
                                    boat_render.attachments.rudder));
    gfx::render_chunk(boat_render.rudder);
    shader.set_model(glm::translate(boat_render.model,
                                    boat_render.attachments.gun));
    gfx::render_chunk(boat_render.gun);
  }

  void Boat_Effect::init(gfx::IDriver& driver,
                         po::variables_map const& vm) noexcept
  {
    // Initialize our basic object shader.
    shader_ = driver.make_shader_repr();
    shader_->load_vertex_part("shader/basic/vs.glsl");
    shader_->load_fragment_part("shader/basic/fs.glsl");

    // We need to get rid of dependency on these in the future.
    shader_->set_diffuse_name("dif");
    shader_->set_projection_name("proj");
    shader_->set_view_name("view");
    shader_->set_model_name("model");
    shader_->set_sampler_name("tex");

    light_pos_loc_ = shader_->get_location("light_pos");

    // Init boat object
    auto& mesh_cache = *driver.get_active_mesh_cache();

    // Figure out what hulls, sails, rudders, and guns we have available.
    boat_descs_ = build_default_descs(mesh_cache);
    log_boat_descs(boat_descs_);

    boat_config_.hull = &boat_descs_.hull_descs[vm["hull"].as < unsigned int > ()];
    boat_config_.sail = &boat_descs_.sail_descs[vm["sail"].as<unsigned int>()];
    boat_config_.rudder = &boat_descs_.rudder_descs[vm["rudder"].as<unsigned int>()];
    boat_config_.gun = &boat_descs_.gun_descs[vm["gun"].as<unsigned int>()];

    log_i("Chosen boat configuration: % (Hull), % (Sail), % (Rudder), "
          "% (Gun)", boat_config_.hull->name, boat_config_.sail->name,
          boat_config_.rudder->name, boat_config_.gun->name);

    boat_render_config_ = build_boat_render_config(boat_config_);
  }
  void Boat_Effect::render(gfx::IDriver& driver, gfx::Camera const& cam) noexcept
  {
    driver.use_shader(*shader_);
    use_camera(driver, cam);

    // Use boat_motion to give the boat a model matrix
    boat_render_config_.model =
      glm::translate(glm::mat4(1.0f), motion.displacement.displacement);

    render_boat(*shader_, boat_render_config_);
  }
} }

/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "deferred.h"
#include "../common/log.h"
#include "common.h"
namespace redc { namespace gfx
{
  Deferred_Shading::Deferred_Shading(IDriver& driver)
    : driver_(&driver), active_(false) {}

  Deferred_Shading::~Deferred_Shading()
  {
    uninit();
  }
  void Deferred_Shading::init(Vec<std::size_t> fb_size,
                              Output_Interface const& interface)
  {
    // Load the shader
    shade_ = driver_->make_shader_repr();
    load_vertex_file(*shade_, "../assets/shader/deferred_fb_write.vs");
    load_fragment_file(*shade_, "../assets/shader/deferred_fb_write.fs");

    shade_->link();

    shade_->set_var_tag("position", "u_position");
    shade_->set_var_tag("normal", "u_normal");
    shade_->set_var_tag("color", "u_color");
    shade_->set_var_tag("viewport", "u_viewport");

    shade_->set_var_tag("light_pos", "u_light_pos");
    shade_->set_var_tag("light_power", "u_light_power");
    shade_->set_var_tag("light_diffuse_color", "u_light_diffuse_color");
    shade_->set_var_tag("light_specular_color", "u_light_specular_color");

    shade_->set_var_tag(gfx::tags::view_tag, "u_view");

    shade_->set_vec4("viewport",
                     glm::vec4(0.0f,0.0f, (float) fb_size.x,(float) fb_size.y));
    shade_->set_integer("position", 0);
    shade_->set_integer("normal", 1);
    shade_->set_integer("color", 2);

    float quad_data[] = {
      -1.0f, -1.0f,
      -1.0f, +1.0f,
      +1.0f, -1.0f,

      -1.0f, +1.0f,
      +1.0f, +1.0f,
      +1.0f, -1.0f
    };

    quad_buf_ = driver_->make_buffer_repr();
    quad_buf_->allocate(Buffer_Target::Array, sizeof(quad_data), quad_data,
                        Usage_Hint::Draw, Upload_Hint::Static);

    quad_ = driver_->make_mesh_repr();
    quad_->format_buffer(*quad_buf_, 0, Attrib_Type::Vec2, Data_Type::Float, 0, 0);
    quad_->enable_attrib_bind(0);
    quad_->set_primitive_type(Primitive_Type::Triangles);

    // TODO: Be smart and try to keep the framebuffer
    uninit();

    fbo_ = driver_->make_framebuffer_repr();

    driver_->bind_framebuffer(*fbo_, Fbo_Binding::Draw);

    for(std::size_t i = 0; i < interface.attachments.size(); ++i)
    {
      Attachment attachment = interface.attachments[i];

      Texture_Format iformat = get_attachment_internal_format(attachment);

      // Make a new texture or render buffer
      if(attachment.type == Attachment_Type::Color)
      {
        // Use a texture
        std::unique_ptr<ITexture> tex = driver_->make_texture_repr();

        Texture_Target target = Texture_Target::Tex_2D;

        tex->allocate(fb_size, iformat, target);

        tex->set_mag_filter(Texture_Filter::Linear);
        tex->set_min_filter(Texture_Filter::Linear);
        tex->set_wrap_s(Texture_Wrap::Clamp_To_Edge);
        tex->set_wrap_t(Texture_Wrap::Clamp_To_Edge);

        // Disable mipmapping
        tex->set_mipmap_level(1);

        fbo_->attach(attachment, *tex);

        // This is a color attachment with a given index, so we need to use it
        // when it comes time to render.
        draw_buffers_.push_back(to_draw_buffer(attachment));

        // Add the texture
        texs_.push_back(std::move(tex));
      }
      else
      {
        // Non color, use a renderbuffer
        std::unique_ptr<IRenderbuffer> rb = driver_->make_renderbuffer_repr();
        rb->define_storage(iformat, fb_size);

        fbo_->attach(attachment, *rb);

        rbs_.push_back(std::move(rb));
      }
    }

    Fbo_Status status = fbo_->status();
    if(status != Fbo_Status::Complete)
    {
      // Unbind then delete
      log_e("Failed to complete framebuffer for deferred rendering: %",
            fbo_status_string(status));
      fbo_.reset(nullptr);
    }
  }
  void Deferred_Shading::uninit()
  {
    finish();

    fbo_.reset(nullptr);

    texs_.clear();
    rbs_.clear();
  }

  void Deferred_Shading::use()
  {
    // TODO: Combine these two functions, I think.
    driver_->bind_framebuffer(*fbo_, Fbo_Binding::Draw);
    driver_->use_framebuffer_draw_buffers(
      draw_buffers_.size(), &draw_buffers_[0]
    );

    // Clear color and depth of the framebuffer.
    driver_->clear();

    // Blending / transparency doesn't work here.
    driver_->blending(false);

    active_ = true;
  }
  void Deferred_Shading::finish()
  {
    driver_->use_default_draw_buffers();

    active_ = false;
  }
  void Deferred_Shading::render(gfx::Camera const& cam,
                                std::vector<Light> const& lights)
  {
    if(is_active())
    {
      finish();
    }

    driver_->use_shader(*shade_, true);

    shade_->set_mat4(gfx::tags::view_tag, camera_view_matrix(cam));

    // This is super stupid and contrived because we only support an interface
    // that goes position, normal, then color. If we could generate the glsl on
    // the fly there would be no problem.

    // Use the textures in the order they
    // were put in the texs_ vector, this should be the same order they were
    // declared in the output interface and therefore the fragment shader.
    // Everything has to stay in the same order. In this case that order is
    // position, normal, color

    std::array<std::string, 3> names = {"position", "normal", "color"};
    for(std::size_t texture_i = 0; texture_i < texs_.size(); ++texture_i)
    {
      driver_->active_texture(texture_i);
      driver_->bind_texture(*texs_[texture_i], Texture_Target::Tex_2D);

      shade_->set_integer(names[texture_i], texture_i);
    }

    // Time to render the fullscreen quad.

    driver_->face_culling(false);

    // We need additive blending
    driver_->set_blend_policy(gfx::Blend_Policy::Additive);

    for(Light const& light : lights)
    {
      shade_->set_vec3("light_pos", light.pos);
      shade_->set_float("light_power", light.power);
      shade_->set_vec3("light_diffuse_color", light.diffuse_color);
      shade_->set_vec3("light_specular_color", light.specular_color);

      // We need to forcefully do this since we rendering using the new
      // interface, which doesn't go through the driver.
      quad_->draw_arrays(0, 6);
    }
  }
} }

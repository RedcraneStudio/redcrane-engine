/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "deferred.h"
#include "funcs.h"
#include "../common/log.h"
namespace redc { namespace gfx
{
  Deferred_Shading::Deferred_Shading(IDriver& driver)
    : driver_(&driver), active_(false), fbo_inited_(false) {}

  Deferred_Shading::~Deferred_Shading()
  {
    uninit();
  }
  void Deferred_Shading::init(Vec<int> fb_size,
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

    quad_ = driver_->make_mesh_repr();
    IMesh::buf_t data_buf;
    quad_->make_buffers(1, &data_buf);
    quad_->allocate_buffer(data_buf, Buffer_Target::Array, sizeof(quad_data),
                           quad_data, Usage_Hint::Draw, Upload_Hint::Static);
    quad_->format_buffer(data_buf, 0, 2, Data_Type::Float, 0, 0);
    quad_->enable_vertex_attrib(0);
    quad_->set_primitive_type(Primitive_Type::Triangle);

    // TODO: Be smart and try to keep the framebuffer
    uninit();

    make_framebuffers(1, &fbo_);
    fbo_inited_ = true;

    bind_framebuffer(Fbo_Binding::Draw, fbo_);

    for(std::size_t i = 0; i < interface.attachments.size(); ++i)
    {
      Attachment attachment = interface.attachments[i];

      // Make a new texture or render buffer
      if(attachment.type == Attachment_Type::Color)
      {
        // Use a texture
        Texture_Repr tex;
        make_textures(1, &tex);

        Texture_Target target = Texture_Target::Tex_2D;

        Texture_Format iformat = get_attachment_internal_format(attachment);
        allocate_texture(tex, target, iformat, fb_size.x, fb_size.y);

        tinygltf::Sampler sampler;
        sampler.magFilter = TINYGLTF_TEXTURE_FILTER_LINEAR;
        sampler.minFilter = TINYGLTF_TEXTURE_FILTER_LINEAR;
        sampler.wrapS = TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE;
        sampler.wrapT = TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE;
        set_sampler(tex, target, sampler);

        // Disable mipmapping
        set_mipmap_level(tex, target, 0);

        framebuffer_attach_texture(Fbo_Binding::Draw, attachment, target, tex);

        // This is a color attachment with a given index, so we need to use it
        // when it comes time to render.
        draw_buffers_.push_back(to_draw_buffer(attachment));

        // Add the texture
        texs_.push_back(tex);
        // Add this as a sampler parameter for the shader that composites the
        // textures to a fullscreen quad.
        Param_Value tex_val;
        tex_val.uint = i;
        params_.push_back(tex_val);
      }
      else
      {
        // Non color, use a renderbuffer
        Renderbuffer_Repr rb;
        make_renderbuffers(1, &rb);

        bind_renderbuffer(rb);
        renderbuffer_storage(attachment, fb_size.x, fb_size.y);

        framebuffer_attach_renderbuffer(Fbo_Binding::Draw, attachment, rb);

        rbs_.push_back(rb);
      }
    }

    Fbo_Status status = check_framebuffer_status(Fbo_Binding::Draw);
    if(status != Fbo_Status::Complete)
    {
      // Unbind then delete
      log_e("Failed to complete framebuffer for deferred rendering: %",
            fbo_status_string(status));
      uninit();
    }
    else
    {
      // Just unbind the framebuffer
      unbind_framebuffer();
    }
  }
  void Deferred_Shading::uninit()
  {
    finish();

    destroy_framebuffers(fbo_inited_ ? 1 : 0, &fbo_);
    fbo_inited_ = false;

    destroy_textures(texs_.size(), &texs_[0]);
    texs_.clear();

    destroy_renderbuffers(rbs_.size(), &rbs_[0]);
    rbs_.clear();
  }

  void Deferred_Shading::use()
  {
    bind_framebuffer(Fbo_Binding::Draw, fbo_);
    set_draw_buffers(draw_buffers_.size(), &draw_buffers_[0]);
    driver_->clear();
    driver_->set_blend_policy(gfx::Blend_Policy::Transparency);
    driver_->blending(false);

    active_ = true;
  }
  void Deferred_Shading::finish()
  {
    // Reset draw buffers and unbind the framebuffer
    unbind_framebuffer();

    Draw_Buffer buf;
    buf.type = Draw_Buffer_Type::Back_Left;
    set_draw_buffers(1, &buf);

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
    Param_Bind pos_bind = shade_->get_tag_bind("position");
    Param_Bind norm_bind = shade_->get_tag_bind("normal");
    Param_Bind color_bind = shade_->get_tag_bind("color");

    // Use the textures in the order they were put in the texs_ vector, this
    // should be the same order they were declared in the output interface and
    // therefore the fragment shader. Everything has to stay in the same order.
    // In this case that order is position, normal, color

    int texture_slot = 0;
    set_parameter(pos_bind, Param_Type::Sampler2D, params_[0], texture_slot, texs_);
    set_parameter(norm_bind, Param_Type::Sampler2D, params_[1], texture_slot, texs_);
    set_parameter(color_bind, Param_Type::Sampler2D, params_[2], texture_slot, texs_);

    driver_->face_culling(false);

    driver_->blending(false);
    driver_->set_blend_policy(gfx::Blend_Policy::Additive);
    driver_->depth_test(true);

    for(Light const& light : lights)
    {
      shade_->set_vec3("light_pos", light.pos);
      shade_->set_float("light_power", light.power);
      shade_->set_vec3("light_diffuse_color", light.diffuse_color);
      shade_->set_vec3("light_specular_color", light.specular_color);

      // We need to forcefully do this since we rendering using the new
      // interface, which doesn't go through the driver.
      driver_->bind_mesh(*quad_, true);
      quad_->draw_arrays(0, 6);

      // This has the effect of disabling blending for the first light.
      driver_->blending(true);
    }

    driver_->face_culling(true);
  }

} }

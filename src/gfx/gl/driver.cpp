/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "driver.h"
#include "prep_mesh.h"
#include "prep_tex.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      Driver::Driver() noexcept
      {
        current_shader_ = &standard_shader_;
        current_shader_->use();
      }
      Driver::~Driver() noexcept
      {
        glUseProgram(0);
      }
      void Driver::set_shader(Shader shade) noexcept
      {
        switch(shade)
        {
          case Shader::Standard:
          {
            current_shader_ = &standard_shader_;
          }
          case Shader::Hud:
          {
            current_shader_ = &hud_shader_;
          }
        }
        current_shader_->use();
      }
      void Driver::prepare_mesh(Mesh& mesh) noexcept
      {
        if(meshs_.find(&mesh) == meshs_.end())
        {
          meshs_.emplace(&mesh, Prep_Mesh(mesh));
        }
      }
      void Driver::remove_mesh(Mesh& mesh) noexcept
      {
        auto mesh_find = meshs_.find(&mesh);
        if(mesh_find != meshs_.end())
        {
          meshs_.erase(mesh_find);
        }
      }
      void Driver::render_mesh(Mesh const& mesh) noexcept
      {
        auto mesh_find = meshs_.find(&mesh);
        if(mesh_find != meshs_.end())
        {
          mesh_find->second.bind();
          mesh_find->second.draw();
        }
      }

      void Driver::prepare_texture(Texture& tex) noexcept
      {
        if(textures_.find(&tex) == textures_.end())
        {
          textures_.emplace(&tex, Prep_Tex(tex));
        }
      }
      void Driver::remove_texture(Texture& tex) noexcept
      {
        auto tex_find = textures_.find(&tex);
        if(tex_find != textures_.end())
        {
          textures_.erase(tex_find);
        }
      }
      void Driver::bind_texture(Texture const& tex, unsigned int l) noexcept
      {
        auto tex_find = textures_.find(&tex);
        if(tex_find != textures_.end())
        {
          // Bind the texture to tex unit l.
          tex_find->second.bind(l);
          // Tell the shader/program that our texture is in location l.
          // This will need to change when we start getting adjustable
          // textures, maybe?
          current_shader_->set_texture(l);
        }
      }

      void Driver::set_diffuse(Color const& c) noexcept
      {
        current_shader_->set_diffuse(c);
      }

      void Driver::set_projection(glm::mat4 const& p) noexcept
      {
        current_shader_->set_projection(p);
      }
      void Driver::set_view(glm::mat4 const& v) noexcept
      {
        current_shader_->set_view(v);
      }
      void Driver::set_model(glm::mat4 const& m) noexcept
      {
        current_shader_->set_model(m);
      }
      void Driver::clear_color_value(Color const& c) noexcept
      {
        glClearColor(c.r / (float) 0xff, c.g / (float) 0xff,
                     c.b / (float) 0xff, 1.0);
      }
      void Driver::clear_depth_value(float f) noexcept
      {
        glClearDepth(f);
      }

      void Driver::clear() noexcept
      {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      }
      void Driver::clear_color() noexcept
      {
        glClear(GL_COLOR_BUFFER_BIT);
      }
      void Driver::clear_depth() noexcept
      {
        glClear(GL_DEPTH_BUFFER_BIT);
      }
    }
  }
}

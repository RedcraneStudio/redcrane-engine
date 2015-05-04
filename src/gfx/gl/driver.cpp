/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "driver.h"
#include "mesh.h"
#include "texture.h"
#include "../../common/software_texture.h"
#include "../../common/software_mesh.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      Driver::Driver(Vec<int> size) noexcept : extents_(size)
      {
        current_shader_ = &standard_shader_;
        current_shader_->use();

        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
                            GL_ZERO);
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
            break;
          }
          case Shader::Hud:
          {
            current_shader_ = &hud_shader_;
            break;
          }
        }
        current_shader_->use();
      }
      std::unique_ptr<Mesh> Driver::make_mesh_repr() noexcept
      {
        // Make it and forget about it, we'll just use a dynamic cast for
        // simplicity.
        return std::make_unique<GL_Mesh>();
      }
      void Driver::render_mesh(Mesh& mesh) noexcept
      {
        auto gl_mesh = dynamic_cast<GL_Mesh*>(&mesh);
        if(!gl_mesh) return;
        gl_mesh->bind();
        gl_mesh->draw();
      }

      std::unique_ptr<Texture> Driver::make_texture_repr() noexcept
      {
        // build n' forget.
        return std::make_unique<GL_Texture>();
      }
      void Driver::bind_texture(Texture& tex, unsigned int loc) noexcept
      {
        auto gl_tex = dynamic_cast<GL_Texture*>(&tex);
        if(!gl_tex) return;
        gl_tex->bind(loc);
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
      void Driver::depth_test(bool enable) noexcept
      {
        if(enable) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
      }
      void Driver::blending(bool enable) noexcept
      {
        if(enable) glEnable(GL_BLEND);
        else glDisable(GL_BLEND);
      }
    }
  }
}

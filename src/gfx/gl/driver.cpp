/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "driver.h"
#include "prep_mesh.h"
#include "prep_tex.h"
namespace strat
{
  namespace gfx
  {
    namespace gl
    {
      Driver::Driver() noexcept
      {
        // In the simple world of a single diffuse-shader, just use it for the
        // lifetime of our driver.
        shader_.use();
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
          shader_.set_texture(l);
        }
      }

      void Driver::prepare_material(Material& mat) noexcept
      {
        if(mat.texture) prepare_texture(*mat.texture);
      }
      void Driver::remove_material(Material& mat) noexcept
      {
        if(mat.texture) remove_texture(*mat.texture);
      }
      void Driver::bind_material(Material const& mat) noexcept
      {
        shader_.set_diffuse(mat.diffuse_color);
        if(mat.texture) bind_texture(*mat.texture, 0);
      }

      void Driver::prepare_camera(Camera&) noexcept {}
      void Driver::remove_camera(Camera&) noexcept {}
      void Driver::use_camera(Camera const& cam) noexcept
      {
        shader_.set_view(camera_view_matrix(cam));
        shader_.set_projection(camera_proj_matrix(cam));
      }

      void Driver::set_model(glm::mat4 const& m) noexcept
      {
        shader_.set_model(m);
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

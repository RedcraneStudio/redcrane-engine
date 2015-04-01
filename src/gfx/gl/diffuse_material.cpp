/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "diffuse_material.h"
#include "program_cache.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      // Use the 
      Diffuse_Material::Diffuse_Material() noexcept
        : prog_(load_program("shader/diffuse/decl.json"))
      {
        diffuse_color_loc_ = prog_->get_uniform_location("diffuse");
        diffuse_color_changed_ = true;
      }
      void Diffuse_Material::use() const noexcept
      {
        if(diffuse_color_changed_)
        {
          glUniform3f(diffuse_color_loc_, diffuse_color_.r / (float) 0xff,
                      diffuse_color_.g / (float) 0xff,
                      diffuse_color_.b / (float) 0xff);
          diffuse_color_changed_ = false;
        }

        prog_->use();
      }

      void Diffuse_Material::diffuse_color(Color const& c) noexcept
      {
        if(c != diffuse_color_)
        {
          diffuse_color_changed_ = true;
          diffuse_color_ = c;
        }
      }
      Color const& Diffuse_Material::diffuse_color() const noexcept
      {
        return diffuse_color_;
      }
    }
  }
}

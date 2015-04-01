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
      }
      void Diffuse_Material::use() const noexcept
      {
        if(diffuse_color_changed_)
        {
          glUniform3f(diffuse_color_loc_, diffuse_color_.r / (float) 0xff,
                      diffuse_color_.g / (float) 0xff,
                      diffuse_color_.b / (float) 0xff);
        }

        prog_->use();
      }
    }
  }
}

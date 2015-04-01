/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "diffuse_material.h"
namespace survive
{
  namespace gfx
  {
    namespace gl
    {
      // Use the 
      Diffuse_Material::Diffuse_Material() noexcept
        : prog_(Program::from_files("shader/diffuse/vertex",
                                    "shader/diffuse/fragment"))
      { }
      void Diffuse_Material::use() const noexcept
      {
        if(diffuse_color_changed_)
        {
          // TODO: Set diffuse color uniform.
        }

        prog_.use();
      }
    }
  }
}

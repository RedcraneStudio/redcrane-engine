/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <memory>
#include "iscene_observer.h"
namespace survive
{
  namespace gfx
  {
    struct IMaterial : public IScene_Observer
    {
      virtual ~IMaterial() noexcept {}
      // Accepting a matrix (model) seems incredibly out of place. Then again
      // it is the material after all that stores the program. Could it be
      // the material should hold another name, as it is more encompassing.
      virtual void use(glm::mat4 const&) const noexcept = 0;

      void set_projection(glm::mat4 const& proj) noexcept = 0;
      void set_view(glm::mat4 const& view) noexcept = 0;
    };

    std::unique_ptr<IMaterial> load_material(std::string const&) noexcept;
  }
}

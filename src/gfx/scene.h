/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
#include "iscene_observer.h"
#include "../common/observer_subject.h"
namespace survive
{
  namespace gfx
  {
    /*!
     * \brief Graphical related parameters of the scene.
     */
    struct Scene : public Observer_Subject<IScene_Observer>
    {
      glm::mat4 const& projection_matrix() const noexcept;
      void projection_matrix(glm::mat4 const& p) noexcept;

      glm::mat4 const& view_matrix() const noexcept;
      void view_matrix(glm::mat4 const& v) noexcept;
    private:
      void on_observer_add_(IScene_Observer* obs) const noexcept override;

      glm::mat4 projection_;
      glm::mat4 view_;
    };

    Scene make_isometric_scene() noexcept;
  }
}

/*
 * Copyright (C) 2014 Luke San Antonio
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
      inline glm::mat4 const& projection_matrix() const noexcept;
      inline void projection_matrix(glm::mat4 const& p) noexcept;

      inline glm::mat4 const& view_matrix() const noexcept;
      inline void view_matrix(glm::mat4 const& v) noexcept;
    private:
      void on_observer_add_(IScene_Observer* obs) const noexcept override;

      glm::mat4 projection_;
      glm::mat4 view_;
    };

    glm::mat4 const& Scene::projection_matrix() const noexcept
    {
      return projection_;
    }
    void Scene::projection_matrix(glm::mat4 const& p) noexcept
    {
      if(p != projection_)
      {
        for(auto obs : observers_)
        {
          obs->set_projection(p);
        }
        projection_ = p;
      }
    }

    glm::mat4 const& Scene::view_matrix() const noexcept
    {
      return view_;
    }
    void Scene::view_matrix(glm::mat4 const& v) noexcept
    {
      if(v != view_)
      {
        for(auto obs : observers_)
        {
          obs->set_view(v);
        }
        view_ = v;
      }
    }
    void Scene::on_observer_add_(IScene_Observer* obs) const noexcept
    {
      // Set both the projection and view matrix on the scene.
      obs->set_projection(projection_);
      obs->set_view(view_);
    }
  }
}

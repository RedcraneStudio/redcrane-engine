/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "scene.h"
#include <glm/gtc/matrix_transform.hpp>
namespace survive
{
  namespace gfx
  {
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

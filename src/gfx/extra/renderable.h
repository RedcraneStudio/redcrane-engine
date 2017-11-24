/*
 * Copyright (C) 2017 Luke San Antonio Bialecki
 * All Rights Reserved.
 */

#ifndef REDC_GFX_EXTRA_RENDERABLE_H
#define REDC_GFX_EXTRA_RENDERABLE_H

#include <glm/glm.hpp>
namespace redc
{
  struct Camera;
  struct IDriver;
  struct Renderable
  {
    virtual ~Renderable() {}

    virtual void init(IDriver& idriver) = 0;

    virtual void render(Camera const& cam) = 0;

    // Is this important?
    virtual glm::mat4 local() const { return this->model(); }
    virtual glm::mat4 model() const = 0;
  };

  // Load "effects" / "techniques" into the driver if necessary.
  struct GLTFAssetRenderable : public Renderable
  {
    void init(IDriver& idriver) override;
    void render(Camera const& cam) override;
    glm::mat4 model() const override;
  };

  // Maybe in the future?
  // struct GLTFNodeRenderable : public Renderable {}

  struct InfiniteFloorRenderable : public Renderable
  {
    void init(IDriver& idriver) override;
    void render(Camera const& cam) override;
    glm::mat4 local() const override { return glm::mat4(1.0f); }
    glm::mat4 model() const override { return glm::mat4(1.0f); }
  };
}

#endif //REDC_GFX_EXTRA_RENDERABLE_H

/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_DEFERRED_RENDERING_H
#define REDC_GFX_DEFERRED_RENDERING_H

#include <vector>

#include "../common/vec.h"

#include "types.h"
#include "idriver.h"
#include "imesh.h"
#include "camera.h"
namespace redc { namespace gfx
{
  struct Output_Interface
  {
    std::vector<Attachment> attachments;
  };

  struct Light
  {
    glm::vec3 pos;
    float power;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
  };

  struct Deferred_Shading
  {
    Deferred_Shading(IDriver& driver);
    ~Deferred_Shading();

    void init(Vec<int> fb_size, Output_Interface const& interface);
    void uninit();

    void use();
    void finish();
    void render(gfx::Camera const& cam, std::vector<Light> const& lights) const;

  private:
    IDriver* driver_;

    std::unique_ptr<gfx::Shader> shade_;
    std::unique_ptr<IMesh> quad_;

    bool fbo_inited_;
    Framebuffer_Repr fbo_;

    std::vector<Param_Value> params_;
    std::vector<Texture_Repr> texs_;
    std::vector<Renderbuffer_Repr> rbs_;

    std::vector<Draw_Buffer> draw_buffers_;
  };
} }

#endif // Header block

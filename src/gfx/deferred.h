/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#ifndef REDC_GFX_DEFERRED_RENDERING_H
#define REDC_GFX_DEFERRED_RENDERING_H

#include <vector>

#include "../common/vec.h"
#include "scene.h"

#include "idriver.h"
#include "imesh.h"
#include "camera.h"
namespace redc { namespace gfx
{
  struct Output_Interface
  {
    std::vector<Attachment> attachments;
  };

  struct Deferred_Shading
  {
    Deferred_Shading(IDriver& driver);
    ~Deferred_Shading();

    void init(Vec<std::size_t> fb_size, Output_Interface const& interface);
    void uninit();

    // Returns true between calls to use() and finish(). After finish() and
    // before use(), is_active() is false.
    bool is_active() const { return active_; }

    void use();
    void finish();
    void render(gfx::Camera const& cam, std::size_t num_lights,
                Transformed_Light* lights);

  private:
    IDriver* driver_;

    bool active_;

    std::unique_ptr<IBuffer> quad_buf_;
    std::unique_ptr<IMesh> quad_;
    std::unique_ptr<IShader> shade_;

    std::unique_ptr<IFramebuffer> fbo_;

    std::vector<std::unique_ptr<ITexture> > texs_;
    std::vector<std::unique_ptr<IRenderbuffer> > rbs_;

    std::vector<Draw_Buffer> draw_buffers_;
  };
} }

#endif // Header block

/*
 * Copyright (c) 2017 Luke San Antonio
 * All rights reserved.
 */

#ifndef RED_CRANE_ENGINE_TEXT_RENDER_H
#define RED_CRANE_ENGINE_TEXT_RENDER_H

#include <string>

#include <glm/glm.hpp>

#include "../idriver.h"

#include "freetype-gl/texture-atlas.h"
#include "freetype-gl/texture-font.h"
#include "freetype-gl/text-buffer.h"

namespace redc
{
  enum class Reference_Point : unsigned int
  {
    Top_Left = 0,
    Top_Center,
    Top_Right,
    Right_Center,
    Bottom_Right,
    Bottom_Center,
    Bottom_Left,
    Left_Center,
    Center,
  };

  struct Text_Render_Ctx
  {
    explicit Text_Render_Ctx(std::string font_path);
    ~Text_Render_Ctx();

    void render_text(gfx::IDriver& driver, std::string text, glm::vec2 pt,
                     Reference_Point ref_pt);
  private:
    std::unique_ptr<gfx::IShader> shader_;

    ftgl::texture_atlas_t *atlas_;
    std::unique_ptr<gfx::ITexture> atlas_tex_;

    ftgl::texture_font_t *font_;
  };
}

#endif

/*
 * Copyright (c) 2017 Luke San Antonio
 * All rights reserved.
 */

#include <freetype-gl/vec234.h>
#include "freetype-gl/vec234.h"
#include "../../common/log.h"
#include "text_render.h"

namespace redc
{
  Text_Render_Ctx::Text_Render_Ctx(std::string font_path)
  {
    atlas_ = ftgl::texture_atlas_new(512, 512, 1);
    font_ = ftgl::texture_font_new_from_file(atlas_, 57, font_path.c_str());
  }
  Text_Render_Ctx::~Text_Render_Ctx()
  {
    ftgl::texture_font_delete(font_);
    ftgl::texture_atlas_delete(atlas_);
  }

  void Text_Render_Ctx::render_text(gfx::IDriver& driver, std::string text,
                                    glm::vec2 left)
  {
    if(!shader_)
    {
      shader_ = driver.make_shader_repr();

      // Load source
      load_vertex_file(*shader_, "../assets/shader/text/vs.glsl");
      load_fragment_file(*shader_, "../assets/shader/text/fs.glsl");

      shader_->link();

      shader_->tag_var("atlas");
      shader_->tag_var("viewport");

      auto vec = driver.window_extents();
      shader_->set_vec4("viewport", glm::vec4{0.0f, 0.0f, (float) vec.x,
                                              (float) vec.y});
    }

    ftgl::text_buffer_t *textbuf = ftgl::text_buffer_new();
    ftgl::vec2 pt;
    pt.x = left.x;
    pt.y = left.y;

    ftgl::markup_t mkup;
    mkup.bold = 0;
    mkup.italic = 0;
    mkup.spacing = 0.0f;
    mkup.gamma = 2.2f;
    mkup.foreground_color.r = 1.0f;
    mkup.foreground_color.g = 1.0f;
    mkup.foreground_color.b = 1.0f;
    mkup.foreground_color.a = 1.0f;
    mkup.background_color.r = 0.0f;
    mkup.background_color.g = 0.0f;
    mkup.background_color.b = 0.0f;
    mkup.background_color.a = 0.0f;
    mkup.underline = 0;
    mkup.strikethrough = 0;
    mkup.font = font_;

    ftgl::text_buffer_add_text(textbuf, &pt,
                               &mkup, text.c_str(),
                               text.length());

    mkup.foreground_color.r = 0.0f;
    ftgl::text_buffer_add_char(textbuf, &pt, &mkup, "!", "r");

    if(!atlas_tex_)
    {
      atlas_tex_ = driver.make_texture_repr();
      atlas_tex_->allocate({atlas_->width, atlas_->height},
                           gfx::Texture_Format::Red);

      atlas_tex_->set_mag_filter(gfx::Texture_Filter::Linear);
      atlas_tex_->set_min_filter(gfx::Texture_Filter::Linear);
      atlas_tex_->set_wrap_s(gfx::Texture_Wrap::Clamp_To_Edge);
      atlas_tex_->set_wrap_t(gfx::Texture_Wrap::Clamp_To_Edge);
    }

    atlas_tex_->blit_tex2d_data(
            {{0,0}, atlas_->width, atlas_->height}, gfx::Texture_Format::Red,
            gfx::Data_Type::UByte, atlas_->data
    );
    shader_->set_integer("atlas", 0);
    driver.active_texture(0);
    driver.bind_texture(*atlas_tex_, gfx::Texture_Target::Tex_2D);
    driver.blending(true);
    driver.set_blend_policy(gfx::Blend_Policy::Transparency);
    ftgl::vertex_buffer_render(textbuf->buffer, GL_TRIANGLES);
  }
}
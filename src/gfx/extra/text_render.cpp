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
                                    glm::vec2 pt, Reference_Point ref_pt)
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
      shader_->tag_var("translation");

      auto vec = driver.window_extents();
      shader_->set_vec4("viewport", glm::vec4{0.0f, 0.0f, (float) vec.x,
                                              (float) vec.y});
    }

    ftgl::text_buffer_t *textbuf = ftgl::text_buffer_new();
    ftgl::vec2 pen{0.0f, 0.0f};

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

    ftgl::text_buffer_add_text(textbuf, &pen,
                               &mkup, text.c_str(),
                               text.length());

    // Figure out alignment
    ftgl::vec4 bounds = ftgl::text_buffer_get_bounds(textbuf, &pen);

    glm::vec2 translation{0.0f, 0.0f};

    // Without adjustment, the top left corner is the origin of the vertices.
    if(ref_pt == Reference_Point::Bottom_Left ||
       ref_pt == Reference_Point::Bottom_Center ||
       ref_pt == Reference_Point::Bottom_Right)
    {
      // Shift everything up by the height, since we want to move relative to
      // the bottom of the text.
      translation.y += bounds.height;
    }

    // Translate x
    if(ref_pt == Reference_Point::Top_Center ||
       ref_pt == Reference_Point::Bottom_Center)
    {
      translation.x -= bounds.width / 2.0;
    }
    else if(ref_pt == Reference_Point::Top_Right ||
            ref_pt == Reference_Point::Bottom_Right)
    {
      translation.x -= bounds.width;
    }

    // Horizontally aligned / centered
    if(ref_pt == Reference_Point::Left_Center ||
       ref_pt == Reference_Point::Right_Center ||
       ref_pt == Reference_Point::Center)
    {
      translation.y += bounds.height / 2.0f;
    }

    // Right aligned at that?
    if(ref_pt == Reference_Point::Right_Center)
    {
      translation.x -= bounds.width;
    }
    else if(ref_pt == Reference_Point::Center)
    {
      translation.x -= bounds.width / 2.0f;
    }

    // Normalize the translation based on the viewport, then add our
    // user-provided translation (which should already be normalized).
    glm::vec2 viewport(driver.window_extents().x, driver.window_extents().y);
    translation = translation / viewport + pt;

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
    shader_->set_vec2("translation", translation);
    driver.active_texture(0);
    driver.bind_texture(*atlas_tex_, gfx::Texture_Target::Tex_2D);
    driver.blending(true);
    driver.set_blend_policy(gfx::Blend_Policy::Transparency);
    ftgl::vertex_buffer_render(textbuf->buffer, GL_TRIANGLES);
  }
}

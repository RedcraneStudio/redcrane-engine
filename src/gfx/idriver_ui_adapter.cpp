/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "idriver_ui_adapter.h"
namespace game { namespace gfx
{
  void set_rect(Mesh& rect, Volume<int> vol)
  {
    // Don't worry about texture coordinates right now.
    rect.set_vertex(0, Vertex{glm::vec3{vol.pos.x, vol.pos.y, 0.0},
                              glm::vec3{0.0, 0.0f, -1.0},
                              glm::vec2{0.0, 0.0}});
    rect.set_vertex(1, Vertex{glm::vec3{vol.pos.x + vol.width, 0.0, 0.0},
                              glm::vec3{0.0, 0.0f, -1.0},
                              glm::vec2{1.0, 0.0}});
    rect.set_vertex(2, Vertex{glm::vec3{vol.pos.x + vol.width,
                                        vol.pos.y + vol.height, 0.0},
                              glm::vec3{0.0, 0.0f, -1.0},
                              glm::vec2{1.0, 1.0}});
    rect.set_vertex(3, Vertex{glm::vec3{0.0, vol.pos.y + vol.height, 0.0},
                              glm::vec3{0.0, 0.0f, -1.0},
                              glm::vec2{0.0, 1.0}});
  };

  IDriver_UI_Adapter::IDriver_UI_Adapter(IDriver& d) noexcept : d_(&d)
  {
    // Prepare an all white texture so we can just change the diffuse color.
    // We can change this when the driver and the shader become more
    // distinguished, right now I'm not really sure how this can be done so
    // this is the simplest option, without adding another shader/program and
    // incurring all that context switching. (Maybe, I'm assuming it's slower
    // then a texture sample, but I could be wrong).
    white_texture_ = d_->make_texture_repr();
    white_texture_->allocate(Vec<int>{1,1});
    white_texture_->blit_data({{0,0}, 1, 1}, &colors::white);

    // Prepare some mesh we can use to render textures.
    filled_rect_.set_impl(d_->make_mesh_repr());
    filled_rect_.allocate(4, 6, Usage_Hint::Draw, Upload_Hint::Stream,
                          Primitive_Type::Triangle);

    auto filled_indices = std::array<unsigned int, 6>{0, 1, 2, 0, 2, 3};
    filled_rect_.set_element_indices(0, 6, &filled_indices[0]);

    lines_rect_.set_impl(d_->make_mesh_repr());
    lines_rect_.allocate(4, 4, Usage_Hint::Draw, Upload_Hint::Stream,
                         Primitive_Type::Line);
    auto line_indices = std::array<unsigned int, 4>{0, 1, 2, 3};
    lines_rect_.set_element_indices(0, 4, &line_indices[0]);
  }

  void IDriver_UI_Adapter::set_draw_color(Color const& c) noexcept
  {
    d_->set_diffuse(c);
    dif_ = c;
  }

  void IDriver_UI_Adapter::draw_rect(Volume<int> const& vol) noexcept
  {
    d_->bind_texture(*white_texture_, 0);

    set_rect(lines_rect_, vol);
    d_->render_mesh(lines_rect_.get_impl());
  }
  void IDriver_UI_Adapter::fill_rect(Volume<int> const& vol) noexcept
  {
    d_->bind_texture(*white_texture_, 0);

    set_rect(filled_rect_, vol);
    d_->render_mesh(filled_rect_.get_impl());
  }

  std::shared_ptr<Texture> IDriver_UI_Adapter::make_texture() noexcept
  {
    return std::shared_ptr<Texture>(d_->make_texture_repr().release());
  }

  void IDriver_UI_Adapter::draw_texture(Volume<int> const& dst, Texture& tex,
                                        Volume<int> const&) noexcept
  {
    // Set the diffuse color.
    d_->set_diffuse(colors::white);

    // Bind the texture.
    d_->bind_texture(tex, 0);

    // Render the rectangle.
    set_rect(filled_rect_, dst);
    d_->render_mesh(filled_rect_.get_impl());

    // Reset the diffuse color
    d_->set_diffuse(dif_);
  }

  void IDriver_UI_Adapter::begin_draw() noexcept
  {
    d_->set_shader(Shader::Hud);
  }
  void IDriver_UI_Adapter::end_draw() noexcept
  {
    d_->set_shader(Shader::Standard);
  }
} }

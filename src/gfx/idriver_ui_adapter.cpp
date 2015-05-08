/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "idriver_ui_adapter.h"
namespace game { namespace gfx
{
  void IDriver_UI_Adapter::set_rect_(Mesh& rect, Volume<int> vol) noexcept
  {
    set_rect_(rect, vol, Volume<int>{{0, 0}, 1, 1}, {1,1});
  }
  void IDriver_UI_Adapter::set_rect_(Mesh& rect, Volume<int> v,
                                     Volume<int> t_v,
                                     Vec<int> t_m) const noexcept
  {
    Volume<float> vol = volume_cast<float>(v);

    auto size = d_->window_extents();
    vol.pos.x = ((vol.pos.x / size.x) - 0.5f) * 2.0f;
    vol.pos.y = ((vol.pos.y / size.y) - 0.5f) * -2.0f;
    vol.width *= 2.0f / size.x;
    vol.height *= 2.0f / -size.y;

    auto tex_v = volume_cast<float>(t_v);
    tex_v.pos.x /= t_m.x;
    tex_v.pos.y /= t_m.y;
    tex_v.width /= t_m.x;
    tex_v.height /= t_m.y;

    rect.set_vertex(0, Vertex{glm::vec3{vol.pos.x,
                                        vol.pos.y, 0.0},
                              glm::vec3{0.0, 0.0f, -1.0},
                              glm::vec2{tex_v.pos.x, tex_v.pos.y}});
    rect.set_vertex(1, Vertex{glm::vec3{vol.pos.x + vol.width,
                                        vol.pos.y, 0.0},
                              glm::vec3{0.0, 0.0f, -1.0},
                              glm::vec2{tex_v.pos.x + tex_v.width,
                                        tex_v.pos.y}});
    rect.set_vertex(2, Vertex{glm::vec3{vol.pos.x + vol.width,
                                        vol.pos.y + vol.height, 0.0},
                              glm::vec3{0.0, 0.0f, -1.0},
                              glm::vec2{tex_v.pos.x + tex_v.width,
                                        tex_v.pos.y + tex_v.height}});
    rect.set_vertex(3, Vertex{glm::vec3{vol.pos.x,
                                        vol.pos.y + vol.height, 0.0},
                              glm::vec3{0.0, 0.0f, -1.0},
                              glm::vec2{tex_v.pos.x,
                                        tex_v.pos.y + tex_v.height}});
  };

  IDriver_UI_Adapter::IDriver_UI_Adapter(IDriver& d) noexcept : d_(&d)
  {
    // Prepare an all white texture so we can just change the diffuse color.
    // We can change this when the driver and the shader become more
    // distinguished, right now I'm not really sure how this can be done so
    // this is the simplest option, without adding another shader/program.
    white_texture_ = d_->make_texture_repr();
    white_texture_->allocate(Vec<int>{1,1});
    white_texture_->blit_data({{0,0}, 1, 1}, &colors::white);

    // Prepare some mesh we can use to render textures.
    filled_rect_.set_impl(d_->make_mesh_repr());
    filled_rect_.allocate(4, 6, Usage_Hint::Draw, Upload_Hint::Stream,
                          Primitive_Type::Triangle);
    filled_rect_.set_num_element_indices(6);

    auto filled_indices = std::array<unsigned int, 6>{0, 1, 2, 0, 2, 3};
    filled_rect_.set_element_indices(0, 6, &filled_indices[0]);

    lines_rect_.set_impl(d_->make_mesh_repr());
    lines_rect_.allocate(4, 8, Usage_Hint::Draw, Upload_Hint::Stream,
                         Primitive_Type::Line);
    lines_rect_.set_num_element_indices(8);
    auto line_indices = std::array<unsigned int, 8>{0, 1, 1, 2, 2, 3, 3, 0};
    lines_rect_.set_element_indices(0, 8, &line_indices[0]);
  }

  void IDriver_UI_Adapter::set_draw_color(Color const& c) noexcept
  {
    d_->set_diffuse(c);
    dif_ = c;
  }

  void IDriver_UI_Adapter::draw_rect(Volume<int> const& vol) noexcept
  {
    d_->bind_texture(*white_texture_, 0);

    set_rect_(lines_rect_, vol, Volume<int>{{1,1},0,0}, Vec<int>{2,2});
    d_->render_mesh(*lines_rect_.get_impl());
  }
  void IDriver_UI_Adapter::fill_rect(Volume<int> const& vol) noexcept
  {
    d_->bind_texture(*white_texture_, 0);

    set_rect_(filled_rect_, vol, Volume<int>{{1,1},0,0}, Vec<int>{2,2});
    d_->render_mesh(*filled_rect_.get_impl());
  }

  std::unique_ptr<Texture> IDriver_UI_Adapter::make_texture() noexcept
  {
    return d_->make_texture_repr();
  }

  void IDriver_UI_Adapter::draw_texture(Volume<int> const& dst, Texture& tex,
                                        Volume<int> const& src) noexcept
  {
    // Set the diffuse color.
    // d_->set_diffuse(colors::white);
    // ^ We shouldn't be setting this to white in case the user does actually
    // want color modulation on the texture (how we are rendering text right
    // now).

    // Bind the texture.
    d_->bind_texture(tex, 0);

    // Render the rectangle.
    set_rect_(filled_rect_, dst, src, tex.allocated_extents());
    d_->render_mesh(*filled_rect_.get_impl());

    // Reset the diffuse color
    d_->set_diffuse(dif_);
  }

  void IDriver_UI_Adapter::begin_draw() noexcept
  {
    d_->depth_test(false);
    d_->blending(true);
    d_->set_shader(Shader::Hud);
  }
  void IDriver_UI_Adapter::end_draw() noexcept
  {
    d_->depth_test(true);
    d_->blending(false);
    d_->set_shader(Shader::Standard);
  }
} }

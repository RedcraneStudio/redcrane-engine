/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "material.h"
#include "../common/json.h"
#include "idriver.h"
namespace game
{
  namespace gfx
  {
    template <class T>
    Color load_color(T const& doc) noexcept
    {
      return {(uint8_t) doc["r"].GetInt(),
              (uint8_t) doc["g"].GetInt(),
              (uint8_t) doc["b"].GetInt()};
    }

    Material load_material(std::string const& s) noexcept
    {
      auto doc = load_json(s);

      auto ret = Material{};

      if_has_member(doc, "diffuse", [&](auto const& val)
      {
        ret.diffuse_color = load_color(val);
      });
      if_has_member(doc, "texture", [&](auto const& val)
      {
        auto texture = Texture::from_png_file(val.GetString());
        ret.texture = Maybe_Owned<Texture>(std::move(texture));
      });

      return ret;
    }
    void prepare_material(IDriver& driver, Material const& mat) noexcept
    {
      if(mat.texture) driver.prepare_texture(*mat.texture);
    }
    void remove_material(IDriver& driver, Material const& mat) noexcept
    {
      if(mat.texture) driver.remove_texture(*mat.texture);
    }
    void bind_material(IDriver& driver, Material const& mat) noexcept
    {
      driver.set_diffuse(mat.diffuse_color);
      // TODO ADD **SOMETHING** TO SUPPORT MORE THAN JUST ONE FREAKIN' TEXTURE!
      if(mat.texture) driver.bind_texture(*mat.texture, 0);
    }
  }
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "material.h"
#include "../common/json.h"
#include "../common/texture_load.h"
#include "../common/software_texture.h"
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
        // Eventually make this a composite texture.
        ret.texture.set_owned(new Software_Texture());
        load_png(val.GetString(), *ret.texture.get());
      });

      return ret;
    }
    void bind_material(IDriver& driver, Material const& mat) noexcept
    {
      driver.set_diffuse(mat.diffuse_color);
      // TODO ADD **SOMETHING** TO SUPPORT MORE THAN JUST ONE FREAKIN' TEXTURE!
      if(mat.texture) driver.bind_texture(*mat.texture, 0);
    }
  }
}

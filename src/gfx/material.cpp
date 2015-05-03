/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "material.h"
#include "../common/json.h"
#include "../common/texture_load.h"
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

    Material::Material() noexcept
    {
      texture = make_maybe_owned<Software_Texture>();
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
        load_png(val.GetString(), *ret.texture);
      });

      return ret;
    }
    void prepare_material(IDriver& driver, Material& mat) noexcept
    {
      mat.texture->set_impl(driver.make_texture_repr(), true);
    }
    void bind_material(IDriver& driver, Material const& mat) noexcept
    {
      driver.set_diffuse(mat.diffuse_color);
      // TODO ADD **SOMETHING** TO SUPPORT MORE THAN JUST ONE FREAKIN' TEXTURE!
      if(mat.texture->get_impl())
      {
        driver.bind_texture(*mat.texture->get_impl(), 0);
      }
    }
  }
}

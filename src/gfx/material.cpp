/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "material.h"
#include "../common/json.h"
#include "prepared_texture.h"
namespace survive
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

    Material load_material(gfx::IDriver& driver,
                           std::string const& s) noexcept
    {
      auto doc = load_json(s);

      auto ret = Material{};

      if_has_member(doc, "diffuse", [&](auto const& val)
      {
        ret.diffuse_color = load_color(val);
      });
      if_has_member(doc, "texture", [&](auto const& val)
      {
        // We should consult a cache of textures already prepared.
        // In fact it may even be useful to implement this in the driver.
        auto texture = Texture::from_png_file(val.GetString());
        auto prep_tex = driver.prepare_texture(std::move(texture));
        ret.texture = std::move(prep_tex);
      });

      return ret;
    }
  }
}

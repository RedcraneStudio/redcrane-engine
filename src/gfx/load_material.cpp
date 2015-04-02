/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "imaterial.h"
#include "../common/json.h"
#include "../common/log.h"
#include "gl/diffuse_material.h"
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

    std::unique_ptr<IMaterial> load_material(gfx::IDriver& driver,
                                             std::string const& s) noexcept
    {
      auto doc = load_json(s);
      auto mat_type = std::string{doc["type"].GetString()};

      auto mat_ptr = std::unique_ptr<IMaterial>{};

      if(mat_type == "diffuse")
      {
        auto diffuse_ptr = std::make_unique<gl::Diffuse_Material>();
        diffuse_ptr->diffuse_color(load_color(doc["color"]));

        if_has_member(doc, "texture", [&](auto const& val)
        {
          auto texture = Texture::from_png_file(val.GetString());
          auto tex = driver.prepare_texture(std::move(texture));
          diffuse_ptr->texture(std::move(tex));
        });
        mat_ptr = std::move(diffuse_ptr);
      }
      else
      {
        log_w("Invalid material type: '%'", mat_type);
      }

      return mat_ptr;
    }
  }
}

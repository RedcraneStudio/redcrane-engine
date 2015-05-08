/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "load.h"
#include "../common/json.h"
#include "../common/texture_load.h"
#include "elements/layouts/linear_layout.h"
#include "elements/layouts/side_layout.h"
#include "elements/layouts/grid_layout.h"
#include "elements/label.h"
#include "elements/empty.h"
#include "elements/sprite.h"
#include "elements/bar.h"
namespace game { namespace ui
{
  template <class T>
  std::string typeof(T const& obj) noexcept
  {
    return obj["type"].GetString();
  }

  template <class T>
  Linear_Layout::Orient orientof(T const& doc)
  {
    if(strcmp(doc["orientation"].GetString(), "vertical") == 0)
    {
      return Linear_Layout::Orient::Vertical;
    }
    if(strcmp(doc["orientation"].GetString(), "horizontal") == 0)
    {
      return Linear_Layout::Orient::Horizontal;
    }
    throw Bad_Orientation{};
  }

  template <class T>
  Vec<int> load_size(T const& doc)
  {
    auto ret = Vec<int>{};

    ret.x = doc["width"].GetInt();
    ret.y = doc["height"].GetInt();

    return ret;
  }

  template <class T>
  Padding load_padding(T const& doc)
  {
    auto ret = Padding{};
    if(!doc.HasMember("padding")) return ret;

    auto const& pad_obj = doc["padding"];
    if_has_member(pad_obj, "left",
    [&](auto const& val)
    {
      ret.left = val.GetInt();
    });
    if_has_member(pad_obj, "right",
    [&](auto const& val)
    {
      ret.right = val.GetInt();
    });
    if_has_member(pad_obj, "top",
    [&](auto const& val)
    {
      ret.top = val.GetInt();
    });
    if_has_member(pad_obj, "bottom",
    [&](auto const& val)
    {
      ret.bottom = val.GetInt();
    });

    return ret;
  }

  template <class T>
  Alignment load_alignment(T const& doc)
  {
    auto align = Alignment{};

    // Set defaults.
    align.horizontal = Horizontal_Alignment::Center;
    align.vertical = Vertical_Alignment::Center;

    auto const& align_obj = doc["alignment"];

    if(align_obj.HasMember("vertical"))
    {
      if(strcmp(align_obj["vertical"].GetString(), "top") == 0)
      {
        align.vertical = Vertical_Alignment::Top;
      }
      else if(strcmp(align_obj["vertical"].GetString(), "center") == 0)
      {
        align.vertical = Vertical_Alignment::Center;
      }
      else if(strcmp(align_obj["vertical"].GetString(), "bottom") == 0)
      {
        align.vertical = Vertical_Alignment::Bottom;
      }
      else
      {
        throw Invalid_Alignment{align_obj["vertical"].GetString()};
      }
    }
    if(align_obj.HasMember("horizontal"))
    {
      if(strcmp(align_obj["horizontal"].GetString(), "left") == 0)
      {
        align.horizontal = Horizontal_Alignment::Left;
      }
      else if(strcmp(align_obj["horizontal"].GetString(), "center") == 0)
      {
        align.horizontal = Horizontal_Alignment::Center;
      }
      else if(strcmp(align_obj["horizontal"].GetString(), "right") == 0)
      {
        align.horizontal = Horizontal_Alignment::Right;
      }
      else
      {
        throw Invalid_Alignment{align_obj["vertical"].GetString()};
      }
    }

    return align;
  }

  template <class T>
  Color load_color(T const& doc)
  {
    if(doc.IsString())
    {
      auto color_str = std::string{doc.GetString()};
      if(color_str == "white") return colors::white;
      if(color_str == "black") return colors::black;
      if(color_str == "green") return colors::green;
      if(color_str == "red")   return colors::red;
      if(color_str == "blue")  return colors::blue;
    }

    Color c;

    c.r = doc["red"].GetInt();
    c.g = doc["green"].GetInt();
    c.b = doc["blue"].GetInt();
    c.a = 0xff;

    return c;
  }

  template <class T>
  Shared_Element load_e(Load_Params p, T const& doc) noexcept
  {
    auto ret_ptr = Shared_Element{};

    if(typeof(doc) == "linear_layout")
    {
      Linear_Layout view{};

      view.orientation = orientof(doc);
      if_has_member(doc, "force_fill", [&view](auto const& val)
      {
        view.force_fill = val.GetBool();
      });
      auto const& children = doc["children"];
      for(auto iter = children.Begin(); iter != children.End(); ++iter)
      {
        Linear_Layout_Params layout;
        // Load the weight if provided.
        layout.weight =
                   iter->HasMember("weight") ?  (*iter)["weight"].GetInt() : 1;
        view.push_child(load_e(p, *iter), layout);
      }
      ret_ptr = std::make_shared<Linear_Layout>(std::move(view));
    }
    else if(typeof(doc) == "side_layout")
    {
      Side_Layout view{};

      auto const& children = doc["children"];
      for(auto iter = children.Begin(); iter != children.End(); ++iter)
      {
        Side_Layout_Params layout;

        layout.padding = load_padding(*iter);
        layout.alignment = load_alignment(*iter);

        view.push_child(load_e(p, *iter), layout);
      }

      ret_ptr = std::make_shared<Side_Layout>(std::move(view));
    }
    else if(typeof(doc) == "grid_layout")
    {
      Grid_Layout view{};

      if_has_member(doc, "force_fill_width", [&](auto const& val)
      {
        view.force_fill_width = val.GetBool();
      });
      if_has_member(doc, "force_fill_height", [&](auto const& val)
      {
        view.force_fill_height = val.GetBool();
      });

      auto const& children = doc["children"];
      for(auto iter = children.Begin(); iter != children.End(); ++iter)
      {
        Grid_Layout_Params layout;

        layout.row = (*iter)["row"].GetInt();
        layout.col = (*iter)["col"].GetInt();

        view.push_child(load_e(p, *iter), layout);
      }

      ret_ptr = std::make_shared<Grid_Layout>(std::move(view));
    }
    else if(typeof(doc) == "label")
    {
      Label label{p.font_render};

      label.str(doc["text"].GetString());
      label.size(doc["size"].GetInt());

      label.color({0xff, 0xff, 0xff});
      if_has_member(doc, "color", [&](auto const& val)
      {
        label.color(load_color(val));
      });

      ret_ptr = std::make_shared<Label>(std::move(label));
    }
    else if(typeof(doc) == "sprite")
    {
      auto sprite = Sprite{};

      sprite.src(std::make_shared<Software_Texture>());
      load_png(doc["src"].GetString(), *sprite.src().get());

      if_has_member(doc, "scale", [&](auto const& val)
      {
        sprite.scale(val.GetDouble());
      });
      if_has_member(doc, "color", [&](auto const& val)
      {
        sprite.color(load_color(val));
      });

      ret_ptr = std::make_shared<Sprite>(std::move(sprite));
    }
    else if(typeof(doc) == "bar")
    {
      auto bar = Bar{};

      if_has_member(doc, "max", [&bar](auto const& val)
      { bar.max(val.GetInt()); });
      if_has_member(doc, "cur", [&bar](auto const& val)
      { bar.cur(val.GetInt()); });

      if_has_member(doc, "color", [&bar](auto const& val)
      {
        bar.color(load_color(val));
      });

      ret_ptr = std::make_shared<Bar>(std::move(bar));
    }
    else if(typeof(doc) == "empty")
    {
      ret_ptr = std::make_shared<Empty>();
    }
    else if(typeof(doc) == "embed")
    {
      ret_ptr = load(doc["src"].GetString(), p);
      // This will use the id of the file that inherited (using embed) unless
      // there is no id given in the parent file, in which case the id in the
      // embedded file will be used.
    }

    if(ret_ptr)
    {
      if(doc.HasMember("id"))
      {
        // Set the id if it was in the json.
        ret_ptr->id = doc["id"].GetString();
      }
      if_has_member(doc, "this_background", [&](auto const& val)
      {
        ret_ptr->set_background(ui::Elem_Volume::This, load_color(val));
      });
      if_has_member(doc, "parent_background", [&](auto const& val)
      {
        ret_ptr->set_background(ui::Elem_Volume::Parent, load_color(val));
      });
      if_has_member(doc, "this_border", [&](auto const& val)
      {
        ret_ptr->set_border(ui::Elem_Volume::This, load_color(val));
      });
      if_has_member(doc, "parent_border", [&](auto const& val)
      {
        ret_ptr->set_border(ui::Elem_Volume::Parent, load_color(val));
      });

      if_has_member(doc, "min_size", [&](auto const& val)
      {
        ret_ptr->min_size(load_size(val));
      });

      if_has_member(doc, "handle_events", [&](auto const& val)
      {
        ret_ptr->handle_events(val.GetBool());
      });

      return ret_ptr;
    }

    return nullptr;
  }
  Shared_Element load(std::string name, Load_Params p) noexcept
  {
    return load_e(p, load_json(name));
  }
} }

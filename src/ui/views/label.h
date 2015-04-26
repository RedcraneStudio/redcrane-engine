/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../View.h"
#include "../../game/Font_Renderer.h"

#include "../../common/surface.h"
#include "../../common/template_utility.hpp"
#include "../../common/translate.h"
namespace game { namespace ui
{
  namespace detail
  {
    struct String_Args
    {
      virtual std::string translate(std::string name) const noexcept = 0;
    };

    template <class... Args>
    struct Tuple_String_Args : public String_Args
    {
      template <class... NArgs>
      Tuple_String_Args(NArgs&&... args) noexcept
                        : tup{std::forward<NArgs>(args)...} {}

      std::string translate(std::string name) const noexcept override;

      std::tuple<Args...> tup;
    };

    template <class... Args> std::string Tuple_String_Args<Args...>
    ::translate(std::string name) const noexcept
    {
      // Call the function with a first argument equal to name followed by the
      // contents of the tuple.
      return call(&game::translate<const Args&...>, tup, name);
    }

  }
  struct Label : public View
  {
    Label(Graphics_Desc& g, Font_Renderer& fr) noexcept : View(g), fr_(fr)
    {
      args_.reset(new detail::Tuple_String_Args<>());
    }

    Volume<int> layout_() override;

    inline std::string str() const noexcept { return str_; }
    inline void str(std::string str) noexcept
    {
      if(str_ != str)
      {
        str_ = str;
        invalidate();
      }
    }

    inline void str_name(std::string name) noexcept
    {
      // Add the at sign denoting the name of a string loaded from the lang
      // file.
      name = "@" + name;

      if(name != str_)
      {
        str_ = name;

        // TODO translate the string right here and now to see if we can get
        // away with not invalidating the cache here.
        invalidate();
      }
    }

    template <class... Args>
    inline void str_args(Args&&... args) noexcept
    {
      // ^^ Accept both lvalues and rvalues (so we can possibly move into the
      // tuple).

      // Initialize our arguments.

      // Make the tuple string args tuple without any references, so that we
      // don't have to worry about storing any references that may expire.
      using tuple_string_args_t =
              detail::Tuple_String_Args<std::remove_reference_t<Args>...>;

      // Initialize the tuple with our args, while moving any values that can
      // be moved.
      args_ = std::make_unique<tuple_string_args_t>(
                                                  std::forward<Args>(args)...);
      invalidate();
    }

    inline int size() const noexcept { return size_; }
    inline void size(int size) noexcept
    {
      if(size_ != size)
      {
        size_ = size;
        invalidate();
      }
    }

    inline Color color() const noexcept { return col_; }
    inline void color(Color col) noexcept
    {
      if(col_ != col)
      {
        col_ = col;
        invalidate();
      }
    }

    inline void invalidate() noexcept override
    { texture_cache_.reset(nullptr); }

  private:
    std::string str_ = "";

    std::unique_ptr<detail::String_Args> args_;

    int size_ = 40;
    Color col_ = Color{};

    Font_Renderer& fr_;

    Vec<int> get_minimum_extents_() const noexcept override;
    void render_() const noexcept override;

    mutable Texture_Ptr texture_cache_;
    void gen_cache_() const noexcept;
  };
} }

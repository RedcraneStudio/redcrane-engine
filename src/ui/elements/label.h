/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../element.h"

#include "../../common/template_utility.hpp"
#include "../../common/translate.h"

#include "../ifont_renderer.h"
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
  struct Label : public Element
  {
    Label(IFont_Renderer& font) noexcept : font_(&font) {}

    inline std::string str() const noexcept { return str_; }
    inline void str(std::string str) noexcept
    {
      if(str != str_)
      {
        str_ = str;
        invalidate_cache_();
      }
    }

    inline void str_name(std::string name) noexcept
    {
      // Add the at sign denoting the name of a string loaded from the lang
      // file.
      str("@" + name);
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
      invalidate_cache_();
    }

    inline int size() const noexcept { return size_; }
    inline void size(int size) noexcept
    {
      if(size != size_)
      {
        size_ = size;
        invalidate_cache_();
      }
    }

    inline Color color() const noexcept { return col_; }
    inline void color(Color col) noexcept
    {
      if(col != col_)
      {
        col_ = col;
        invalidate_cache_();
      }
    }

    inline IFont_Renderer& font_renderer() const noexcept { return *font_; }
    inline void font_renderer(IFont_Renderer& f) noexcept
    {
      if(&f != font_)
      {
        font_ = &f;
        invalidate_cache_();
      }
    }

  private:
    std::string full_str_() const noexcept;

    std::string str_ = "";

    std::unique_ptr<detail::String_Args> args_;

    IFont_Renderer* font_;
    mutable std::shared_ptr<Texture> tex_;

    int size_ = 40;
    Color col_ = Color{};

    void invalidate_cache_() const noexcept;
    void gen_cache_(Renderer&) const noexcept;

    Vec<int> get_minimum_extents_() const noexcept override;
    Volume<int> layout_() override;
    void render_(Renderer&) const noexcept override;
  };
} }

/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/Color.h"
#include "../common/volume.h"
#include "../game/Graphics_Desc.h"

#include "cast.h"
#include "Event_Trigger.h"

#include <memory>
#include <vector>

#include <boost/optional.hpp>
namespace game { namespace ui
{
  struct View;
  using Shared_View = std::shared_ptr<View>;

  struct Small_Volume_Error {};

  struct Trigger_Connection
  {
    // Some pointer or something...
    void disconnect() noexcept {}
  };

  enum class View_Volume
  {
    This, Parent
  };

  struct View
  {
    View(Graphics_Desc& graphics) noexcept : graphics_(graphics) {}
    virtual ~View() {}

    virtual bool is_container() const noexcept { return false; }

    inline bool layout(Vec<int>);
    inline bool layout(Volume<int>);

    void render() const noexcept;

    virtual void invalidate() noexcept {}

    Vec<int> get_minimum_extents() const noexcept;

    inline bool dispatch_event(SDL_Event const&) noexcept;

    inline Volume<int> const& parent_volume() const noexcept;
    inline Volume<int> const& this_volume() const noexcept;

    inline Trigger_Connection
    add_event_trigger(std::shared_ptr<Event_Trigger> const&) noexcept;

    template <class T, class... Args>
    inline Trigger_Connection add_event_trigger(Args&&...) noexcept;

    std::string id;

    void set_border(View_Volume, Color color) noexcept;
    boost::optional<Color> query_border(View_Volume) const noexcept;
    bool remove_border(View_Volume) noexcept;

    void set_background(View_Volume, Color color) noexcept;
    boost::optional<Color> query_background(View_Volume) const noexcept;
    bool remove_background(View_Volume) noexcept;

    Vec<int> min_size() const noexcept;
    void min_size(Vec<int>) noexcept;

    inline void visible(bool visible) noexcept;
    inline bool visible() const noexcept;

    inline void handle_events(bool h) noexcept;
    inline bool handle_events() noexcept;

    inline Shared_View
    find_child(std::string id, bool recursive = false) const noexcept;

    inline Shared_View
    find_child_r(std::string id, bool recursive = true) const noexcept;

    template <class T> inline std::shared_ptr<T>
    find_child(std::string, bool = false) const noexcept;

    template <class T> inline std::shared_ptr<T>
    find_child_r(std::string, bool = true) const noexcept;

    inline bool
    replace_child(std::string, Shared_View, bool r = false) noexcept;

    inline bool
    replace_child_r(std::string, Shared_View, bool r = true) noexcept;

    virtual std::vector<Shared_View> children() noexcept { return {}; }
  protected:
    Graphics_Desc& graphics_;
  private:
    Volume<int> parent_vol_;
    Volume<int> this_vol_;

    bool layed_out_ = false;
    bool visible_ = true;
    bool handle_events_ = true;

    virtual bool dispatch_event_(SDL_Event const& event) noexcept;
    virtual Volume<int> layout_() = 0;
    virtual void render_() const noexcept = 0;
    virtual Vec<int> get_minimum_extents_() const noexcept = 0;

    std::vector<std::shared_ptr<Event_Trigger> > event_triggers_;

    boost::optional<Color> this_border_;
    boost::optional<Color> parent_border_;

    boost::optional<Color> this_background_;
    boost::optional<Color> parent_background_;

    Vec<int> min_size_ = {0,0};

    inline virtual Shared_View find_child_(std::string, bool) const noexcept;
    inline virtual bool
    replace_child_(std::string, Shared_View, bool) noexcept;
  };

  inline bool View::dispatch_event(SDL_Event const& e) noexcept
  {
    if(handle_events_) return dispatch_event_(e);
    return false;
  }

  inline Volume<int> const& View::parent_volume() const noexcept
  {
    return parent_vol_;
  }
  inline Volume<int> const& View::this_volume() const noexcept
  {
    return this_vol_;
  }

  inline Trigger_Connection
  View::add_event_trigger(std::shared_ptr<Event_Trigger> const& et) noexcept
  {
    event_triggers_.push_back(std::move(et));
    return {};
  }

  template <class T, class... Args>
  inline Trigger_Connection View::add_event_trigger(Args&&... args) noexcept
  {
    auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
    event_triggers_.push_back(std::move(ptr));
    return {};
  }

  inline void View::visible(bool visible) noexcept
  {
    visible_ = visible;
  }
  inline bool View::visible() const noexcept
  {
    return visible_;
  }
  inline void View::handle_events(bool h) noexcept
  {
    handle_events_ = h;
  }
  inline bool View::handle_events() noexcept
  {
    return handle_events_;
  }

  inline bool View::layout(Vec<int> size)
  {
    return layout({{0, 0}, size.x, size.y});
  }
  inline bool View::layout(Volume<int> vol)
  {
    try
    {
      parent_vol_ = std::move(vol);
      this_vol_ = layout_();
      layed_out_ = true;
    }
    catch(Small_Volume_Error& e)
    {
      return false;
    }
    catch(...)
    {
      throw;
    }
    return true;
  }

  inline Shared_View View::find_child(std::string id, bool r) const noexcept
  {
    return find_child_(id, r);
  }
  inline Shared_View View::find_child_r(std::string id, bool r) const noexcept
  {
    return find_child_(id, r);
  }


  template <class T> inline std::shared_ptr<T>
  View::find_child(std::string id, bool r) const noexcept
  {
    return as<T>(find_child(id, r));
  }
  template <class T> inline std::shared_ptr<T>
  View::find_child_r(std::string id, bool r) const noexcept
  {
    return as<T>(find_child(id, r));
  }

  inline bool
  View::replace_child(std::string i, Shared_View v, bool r) noexcept
  {
    return replace_child_(i, v, r);
  }

  inline bool
  View::replace_child_r(std::string i, Shared_View v, bool r) noexcept
  {
    return replace_child_(i, v, r);
  }

  inline Shared_View View::find_child_(std::string, bool) const noexcept
  {
    return nullptr;
  }
  inline bool View::replace_child_(std::string, Shared_View, bool) noexcept
  {
    return false;
  }

  inline bool View::dispatch_event_(SDL_Event const& event) noexcept
  {
    bool ret = false;
    for(auto shared_trigger : event_triggers_)
    {
      ret = ret || shared_trigger->try_trigger(*this, event);
    }
    return ret;
  }
} }

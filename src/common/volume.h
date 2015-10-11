/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include "center.hpp"
#include "vec.h"
#include <algorithm>

namespace game
{
  template <typename T = int>
  struct Volume
  {
    Vec<T> pos;
    T width;
    T height;
  };

  template <typename T>
  Volume<T> view_pt(Vec<T> const& vp_extents, Vec<T> const& map_extents,
                    Vec<T> const& pt, double map_scale) noexcept
  {
    Volume<T> vol;

    auto const vp_width = T(vp_extents.x / map_scale);
    auto const vp_height = T(vp_extents.y / map_scale);

    // Find the coordinates of the top-left corner of a viewport that results
    // in the player being centered on it.
    // However, don't allow the corner to go behind (0,0).
    vol.pos.x = std::max(T(0), pt.x - vp_width / 2);
    vol.pos.y = std::max(T(0), pt.y - vp_height / 2);

    // The width and height of the viewport are always no greater than the
    // calculated values above (using the map scale and screen size). They will
    // be smaller when the max width would result in a src rectangle that went
    // beyond the map image's bounds.
    vol.width = std::min(vp_width, map_extents.x - vol.pos.x);
    vol.height = std::min(vp_height, map_extents.y - vol.pos.y);

    // Adjust the viewport corners to give the possibly shortened width and
    // height room to be the max.
    vol.pos.x -= vp_width - vol.width;
    vol.pos.y -= vp_height - vol.height;

    // The viewport should always have the calculated width. We know this won't
    // go beyond the map image's bounds because we already adjusted the x and y
    // values accordingly.
    vol.width = vp_width;
    vol.height = vp_height;

    return vol;
  }

  template <typename T, typename P>
  bool is_in(Volume<T> const& vol, Vec<P> const& pt)
  {
    if(vol.pos.x <= pt.x && pt.x <= vol.pos.x + vol.width &&
       vol.pos.y <= pt.y && pt.y <= vol.pos.y + vol.height)
    {
      return true;
    }
    return false;
  }

  template <typename T>
  Volume<T> vol_from_extents(Vec<T> const& v)
  {
    return {{0, 0}, v.x, v.y};
  }
  template <typename T>
  Vec<T> vol_extents(Volume<T> const& vol)
  {
    return {vol.width, vol.height};
  }

  template <typename T>
  Volume<T> center_volume(Volume<T> parent, Vec<T> child_extents)
  {
    parent.pos.x = center<T>(parent.pos.x, parent.width,  child_extents.x);
    parent.pos.y = center<T>(parent.pos.y, parent.height, child_extents.y);

    parent.width = child_extents.x;
    parent.height = child_extents.y;

    return parent;
  }

  template <typename Ret, typename Accept>
  Volume<Ret> volume_cast(Volume<Accept> const& v) noexcept
  {
    Volume<Ret> ret;

    ret.width  = static_cast<Ret>(v.width);
    ret.height = static_cast<Ret>(v.height);
    ret.pos.x  = static_cast<Ret>(v.pos.x);
    ret.pos.y  = static_cast<Ret>(v.pos.y);

    return ret;
  }

  template <typename T>
  Volume<T> contain_inside(Volume<T> vol, Volume<T> const& inside) noexcept
  {
    // Get the actual positions of the right and bottom edge (rather, whatever
    // edges are not represented by the position vector)
    Vec<int> extent_sides{vol.pos.x + vol.width, vol.pos.y + vol.height};

    vol.pos.x = std::max(inside.pos.x, vol.pos.x);
    vol.pos.y = std::max(inside.pos.y, vol.pos.y);

    extent_sides.x = std::min(extent_sides.x, inside.pos.x + inside.width);
    extent_sides.y = std::min(extent_sides.y, inside.pos.y + inside.height);

    vol.width = extent_sides.x - vol.pos.x;
    vol.height = extent_sides.y - vol.pos.y;

    return vol;
  }
  template <typename T>
  Volume<T> contain_inside_extents(Volume<T> vol, Vec<T> const& in) noexcept
  {
    return contain_inside(vol, vol_from_extents(in));
  }

  template <typename T1, typename T2>
  bool operator==(Volume<T1> const& v1, Volume<T2> const& v2) noexcept
  {
    return v1.pos == v2.pos && v1.width == v2.width && v1.height == v2.height;
  }

  template <typename T1, typename T2>
  bool operator!=(Volume<T1> const& v1, Volume<T2> const& v2) noexcept
  {
    return !(v1 == v2);
  }

  /*! Returns the respective quad in a corner of a given volume.
   *
   * \param in_vol The input volume.
   * \param corner The corner of input_volume that must be returned. Possible
   * corners are:<br/>
   * _Top left => 0_<br/>
   * _Top right => 1_<br/>
   * _Bottom left => 2_<br/>
   * _Bottom right => 3_<br/>
   */
  template <class T>
  Volume<T> vol_quad(Volume<T> const& in_vol, short corner) noexcept
  {
    switch(corner)
    {
    case 0:
      return {in_vol.pos, in_vol.width / 2, in_vol.height / 2};
      break;
    case 1:
      return {{in_vol.pos.x / 2, in_vol.pos.y},
              in_vol.width / 2, in_vol.height / 2};
      break;
    case 2:
      return {{in_vol.pos.x, in_vol.pos.y / 2},
              in_vol.width / 2, in_vol.height / 2};
      break;
    case 3:
      return {{in_vol.pos.x / 2, in_vol.pos.y / 2},
              in_vol.width / 2, in_vol.height / 2};
      break;
    default:
      return in_vol;
    }
  }
}

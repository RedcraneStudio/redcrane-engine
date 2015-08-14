/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "wall.h"

#include <algorithm>
#include <cmath>

#include "../common/log.h"

#include <glm/glm.hpp>

namespace game { namespace strat
{
  void use_segment_point(std::size_t& index_ref, Wall& wall,
                         Vec<float> const& pt) noexcept
  {
    auto find_map_end = std::find(begin(wall.points), end(wall.points), pt);
    if(find_map_end == end(wall.points))
    {
      wall.points.push_back(pt);
      index_ref = wall.points.size() - 1;
    }
    else
    {
      index_ref = find_map_end - begin(wall.points);
    }
  }

  void end_wall(Pending_Wall const& pending_wall, Wall& wall,
                Vec<float> map_end) noexcept
  {
    using std::begin; using std::end;

    Wall_Segment segment;
    segment.type = pending_wall.type;

    use_segment_point(segment.indices.first, wall, pending_wall.pos);
    use_segment_point(segment.indices.second, wall, map_end);
  }

  Vec<float> pending_wall_end_pt(Pending_Wall const& pending_wall,
                                 Vec<float> const& map) noexcept
  {
    // The point from the start of the pending wall to the map point.
    auto mouse_dir = normalize(map - pending_wall.pos);

    // Get that angle.
    auto angle = std::atan2(mouse_dir.y, mouse_dir.x);

    Vec<float> dir{};

    if(-M_PI / 4 < angle && angle < M_PI / 4)
    {
      dir.x = 1.0f;
      dir.y = 0.0f;
    }
    else if(M_PI / 4 < angle && angle < M_PI * 3 / 4)
    {
      dir.x = 0.0f;
      dir.y = 1.0f;
    }
    else if(-M_PI * 3 / 4 < angle && angle < -M_PI / 4)
    {
      dir.x = 0.0f;
      dir.y = -1.0f;
    }
    else
    {
      dir.x = -1.0f;
      dir.y = 0.0f;
    }

    return (dir * length(map - pending_wall.pos)) + pending_wall.pos;
  }
#if 0
  void Wall_Structure::on_place(Vec<float> pos) noexcept
  {
    //auto pos = on_snap(pos_in);

    if(snap_)
    {
      // We are currently snapped to some segment of the wall.

      // TODO: Put in either left or right points, this code only adds the
      // right point.
      if(snap_->side == Side::Left)
      {
        // This is the left side of the wall *we are moving*.
        wall_->points.push_back({pos.x + aabb().width / 2.0f, pos.y});
      }
      else
      {
        wall_->points.push_back({pos.x - aabb().width / 2.0f, pos.y});
      }
    }
    else
    {
      wall_->points.push_back({pos.x - aabb().width / 2.0f, pos.y});
      wall_->points.push_back({pos.x + aabb().width / 2.0f, pos.y});
    }

  }
  void Wall_Structure::on_remove(Vec<float> pos) noexcept
  {
  }
  Vec<float> Wall_Structure::on_snap(Vec<float> pos) noexcept
  {
    // Snapping threshold.
    constexpr float threshold = .45f;

    // Find the right and left point.

    auto left_pos = Vec<float>{pos.x - aabb().width / 2.0f, pos.y};
    auto right_pos = Vec<float>{pos.x + aabb().width / 2.0f, pos.y};

    for(std::size_t i = 0; i < wall_->points.size(); ++i)
    {
      auto pt = wall_->points[i];

      if(length(left_pos - pt) < threshold)
      {
        snap_ = Snap{i, Side::Left};
        return {pt.x + aabb().width / 2.0f + .03f, pt.y};
      }
      if(length(right_pos - pt) < threshold)
      {
        snap_ = Snap{i, Side::Right};
        return {pt.x - aabb().width / 2.0f - .03f, pt.y};
      }
    }

    snap_ = boost::none;
    return pos;
  }
#endif
} }

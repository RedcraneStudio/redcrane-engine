/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "wall.h"

#include <algorithm>
#include <cmath>

#include "map.h"
#include "structure.h"

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

    wall.segments.push_back(segment);
  }

  Vec<float> pending_wall_end_pt(Pending_Wall const& pending_wall,
                                 Vec<float> const& map) noexcept
  {
    // Project the point onto an axis aligned with the pending wall origin
    auto pt = project_onto_pt_axes(pending_wall.pos, map);

    // Round the value to the nearest log, distance wise
    auto distance = length(pt - pending_wall.pos);

    auto unit_size = pending_wall.type.unit_size;
    int units = 0;
    while(distance > unit_size)
    {
      distance -= unit_size;
      ++units;
    }
    // Distance is between 0 and unit_size.
    if(distance < unit_size / 2)
    {
      // Distance is closer to 0
      distance = 0;
    }
    else
    {
      // It's closer to unit size.
      distance = unit_size;
    }
    // Add back all that we subtracted to get the final distance.
    distance += unit_size * units;

    return normalize(pt - pending_wall.pos) * distance + pending_wall.pos;
  }

  void render_wall(gfx::IDriver& driver, Wall const& wall,
                   Structure const& wall_struct) noexcept
  {
    for(auto const& segment : wall.segments)
    {
      auto fp = wall.points[segment.indices.first];
      auto sp = wall.points[segment.indices.second];

      auto vec = sp - fp;
      auto distance = length(vec);

      // How many things do we render?
      auto unit_size = segment.type.unit_size;
      auto units = (int) std::round(distance / unit_size);

      auto normal_vec = normalize(vec);
      // TODO: Make this much more efficient
      auto accum = 0.0;
      for(int unit_i = 0; unit_i < units; ++unit_i)
      {
        render_structure(driver, wall_struct, (normal_vec * accum) + fp);
        accum += unit_size;
      }
    }
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

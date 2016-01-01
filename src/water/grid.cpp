/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "grid.h"
#include "../common/log.h"

#include <vector>
#include <glm/glm.hpp>
#include "../common/vec.h"
namespace game { namespace water
{
  glm::vec3 ray_intersection(Plane const& plane, glm::vec3 pt1,
                             glm::vec3 pt2) noexcept
  {
    auto dir = pt2 - pt1;

    // Project the line onto the plane normal.
    float denom = glm::dot(plane.normal, dir);

    float num = plane.dist - glm::dot(plane.normal, pt1);

    float t = num / denom;

    return pt1+ (t * dir);
  }

  std::vector<glm::vec3> project_grid(std::vector<glm::vec2> const& grid,
                                      glm::mat4 projector,
                                      Plane const& p) noexcept
  {
    std::vector<glm::vec3> ret;
    for(auto const& pt : grid)
    {
      auto start = projector * glm::vec4(pt, -1.0f, 1.0f);
      auto end = projector * glm::vec4(pt, +1.0f, 1.0f);

      // Do the intersection with the plane
      ret.push_back(
        ray_intersection(p, glm::vec3(start / start.w), glm::vec3(end / end.w))
      );

      //ret.push_back(glm::vec3(start / start.w));
    }
    return ret;
  }

  std::vector<glm::vec3> find_visible(gfx::Camera& cam, float y,
                                      float max_disp) noexcept
  {
    glm::mat4 inv_cam = glm::inverse(gfx::camera_proj_matrix(cam) *
                                     gfx::camera_view_matrix(cam));

    // Why does this need two curly brace levels?
    std::array<glm::vec4, 8> corners = { {
      inv_cam * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
      inv_cam * glm::vec4(+1.0f, -1.0f, -1.0f, 1.0f),
      inv_cam * glm::vec4(-1.0f, +1.0f, -1.0f, 1.0f),
      inv_cam * glm::vec4(+1.0f, +1.0f, -1.0f, 1.0f),
      inv_cam * glm::vec4(-1.0f, -1.0f, +1.0f, 1.0f),
      inv_cam * glm::vec4(+1.0f, -1.0f, +1.0f, 1.0f),
      inv_cam * glm::vec4(-1.0f, +1.0f, +1.0f, 1.0f),
      inv_cam * glm::vec4(+1.0f, +1.0f, +1.0f, 1.0f)
    } };

    std::array<std::size_t, 24> lines = {
      0, 1,
      0, 2,
      2, 3,
      1, 3,
      0, 4,
      2, 6,
      3, 7,
      1, 5,
      4, 6,
      4, 5,
      5, 7,
      6, 7
    };

    std::vector<glm::vec3> intersections;

    // Build upper and lower planes
    Plane upper{{0.0f, 1.0f, 0.0f}, y - max_disp};
    Plane lower{{0.0f, 1.0f, 0.0f}, y + max_disp};

    for(unsigned int i = 0; i < lines.size() / 2; ++i)
    {
      // Find the lines of the regular camera frustum, since the regular camera
      // obviously defines what is visible.

      // Is it safe to cast to vec3s?
      auto src = corners[lines[i * 2]];
      auto dst = corners[lines[i * 2 + 1]];

      auto src_pt = glm::vec3(src / src.w);
      auto dst_pt = glm::vec3(dst / dst.w);

      // If the line (src_pt; dst_pt) intersects upper
      if((glm::dot(plane_as_vec4(upper), glm::vec4(src_pt, 1.0f)) /
         (glm::dot(plane_as_vec4(upper), glm::vec4(dst_pt, 1.0f)))) < 0)
      {
        intersections.push_back(ray_intersection(upper, src_pt, dst_pt));
      }
      // If the line (src_pt; dst_pt) intersects lower
      if((glm::dot(plane_as_vec4(lower), glm::vec4(src_pt, 1.0f)) /
         (glm::dot(plane_as_vec4(lower), glm::vec4(dst_pt, 1.0f)))) < 0)
      {
        // Find the exact intersection point
        intersections.push_back(ray_intersection(lower, src_pt, dst_pt));
      }
    }
    for(auto const& pt : corners)
    {
      glm::vec4 pt_proj = pt / pt.w;

      // If pt is in between upper and lower planes
      if(glm::dot(plane_as_vec4(upper), pt_proj) /
         glm::dot(plane_as_vec4(lower), pt_proj) < 0)
      {
        intersections.push_back(glm::vec3(pt_proj));
      }
    }

    return intersections;
  }

  glm::mat4 build_projector(gfx::Camera cam, Plane const& p, float disp) noexcept
  {
    Plane lower{p.normal, p.dist - disp};

    float cam_height = glm::dot(p.normal, cam.look_at.eye);

    gfx::Camera projector = cam;

    // If we are under the maximum height of the water: reposition the camera.
    if(cam_height < p.dist + disp)
    {
      // Underwater
      if(cam_height < 0.0f)
      {
        // TODO: Comment this
        projector.look_at.eye += lower.normal * (disp + p.dist - 2 * cam_height);
      }
      else
      {
        projector.look_at.eye += lower.normal * (disp + p.dist - cam_height);
      }
    }

    glm::vec3 aim;

    // If the camera is facing away from the plane xor the camera is
    // underwater. In other words, the camera must either be above ground and
    // facing the opposite direction of the base plane OR facing the same
    // direction of the base plane when underwater. The direction of the plane
    // is obviously the direction of it's normal, which is always up.
    if((glm::dot(plane_as_vec4(p),
                 glm::vec4(camera_forward(cam), 0.0f)) < 0.0f) ^
       (glm::dot(plane_as_vec4(p),
                 glm::vec4(cam.look_at.eye, 1.0f)) < 0.0f))
    {
      // There should be a collision here I think.
      aim = ray_intersection(p, cam.look_at.eye, cam.look_at.look);
    }
    else
    {
      // Flip the camera
      glm::vec3 cam_dir_flipped;
      cam_dir_flipped = camera_forward(cam) - (2.0f * p.normal *
                        glm::dot(p.normal, gfx::camera_forward(cam)));
      aim = ray_intersection(p, cam.look_at.eye, cam.look_at.eye + cam_dir_flipped);
    }

    projector.look_at.look = aim;

    return glm::inverse(gfx::camera_proj_matrix(projector) *
                        gfx::camera_view_matrix(projector));
  }
  glm::mat4 build_min_max_mat(std::vector<glm::vec3>& intersections,
                             glm::mat4 const& projector,
                             Plane const& p) noexcept
  {
    float x_max = intersections[0].x;
    float x_min = x_max;
    float y_max = intersections[0].z;
    float y_min = y_max;
    for(std::size_t i = 0; i < intersections.size(); ++i)
    {
      // Project points onto the water base plane.
      intersections[i] += - p.normal * glm::dot(p.normal, intersections[i]);

      // Re-project them into post-projection space of the projector camera.
      auto pt = glm::inverse(projector) * glm::vec4(intersections[i], 1.0f);
      intersections[i] = glm::vec3(pt / pt.w);

      // Find the minimum and maximum
      if(i == 0)
      {
        x_max = intersections[i].x;
        x_min = intersections[i].x;
        y_max = intersections[i].y;
        y_min = intersections[i].y;
      }
      else
      {
        x_max = std::max(x_max, intersections[i].x);
        x_min = std::min(x_min, intersections[i].x);
        y_max = std::max(y_max, intersections[i].y);
        y_min = std::min(y_min, intersections[i].y);
      }
    }

    return glm::transpose(glm::mat4(
      x_max - x_min, 0.0f,          0.0f, x_min,
      0.0f,          y_max - y_min, 0.0f, y_min,
      0.0f,          0.0f,          1.0f, 0.0f,
      0.0f,          0.0f,          0.0f, 1.0f
    ));
  }

  // Data is just positions, positions are the same as uv coordinates.
  std::vector<glm::vec2> gen_grid(const unsigned int width) noexcept
  {
    std::vector<glm::vec2> data;

    float grid_size = 1.0f / (float) width;

    for(unsigned int i = 0; i < width * width; ++i)
    {
      // Our grid coordinate
      unsigned int x = i % width;
      unsigned int y = i / width;

      // Scale our grid coordinate
      float x_pos = (float) x * grid_size;
      float y_pos = (float) y * grid_size;

      // Push the grid square
      data.push_back(glm::vec2(x_pos,             y_pos            ));
      data.push_back(glm::vec2(x_pos + grid_size, y_pos            ));
      data.push_back(glm::vec2(x_pos + grid_size, y_pos + grid_size));
      data.push_back(glm::vec2(x_pos,             y_pos            ));
      data.push_back(glm::vec2(x_pos + grid_size, y_pos + grid_size));
      data.push_back(glm::vec2(x_pos,             y_pos + grid_size));
    }

    return data;
  }

  // Sobel filter impl. code not used but I want to keep this here.

  template <class T>
  float apply_sobel_kernel_x(Vec<int> pt, Value_Map<T> const& map,
                             float scale = 1.0f) noexcept
  {
    // Code derived from: http://stackoverflow.com/questions/5281261
    // "Generating a normal map from a height map?"

    auto s_2 = map.at({pt.x + 1, pt.y + 1});
    auto s_0 = map.at({pt.x - 1, pt.y + 1});
    auto s_5 = map.at({pt.x + 1, pt.y});
    auto s_3 = map.at({pt.x - 1, pt.y});
    auto s_8 = map.at({pt.x + 1, pt.y - 1});
    auto s_6 = map.at({pt.x - 1, pt.y - 1});
    return scale * -(s_2 - s_0 + 2 * (s_5 - s_3) + s_8 - s_6);
  }
  template <class T>
  float apply_sobel_kernel_y(Vec<int> pt, Value_Map<T> const& map,
                             float scale = 1.0f) noexcept
  {
    auto s_6 = map.at({pt.x - 1, pt.y - 1});
    auto s_0 = map.at({pt.x - 1, pt.y + 1});
    auto s_7 = map.at({pt.x, pt.y - 1});
    auto s_1 = map.at({pt.x, pt.y + 1});
    auto s_8 = map.at({pt.x + 1, pt.y - 1});
    auto s_2 = map.at({pt.x + 1, pt.y + 1});
    return scale * -(s_6 - s_0 + 2 * (s_7 - s_1) + s_8 - s_2);
  }
} }

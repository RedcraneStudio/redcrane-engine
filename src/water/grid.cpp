/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "grid.h"
#include "../common/log.h"

#include <vector>
#include <glm/glm.hpp>
#include "../common/vec.h"

#include <png.h>
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

  Water::Water(int64_t seed, Vec<int> grid_extents) noexcept
  {
    open_simplex_noise(seed, &osn);

    // We know what grid size we want.
    extents = grid_extents;

    // The normal map should match be four times as large as the grid.
    grid_extents *= 4;
    normalmap.allocate(grid_extents);

    // The heightmap should be a little bit bigger so that generating the
    // normalmap with a 3x3 sobol kernel works.
    grid_extents = {grid_extents.x + 2, grid_extents.y + 2};
    heightmap.allocate(grid_extents);
  }
  Water::~Water() noexcept
  {
    open_simplex_noise_free(osn);
  }

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

  void gen_normalmap(Water& water, float scale) noexcept
  {
    // Assert the extents of the normalmap are expected? (in comparison to the
    // heightmap)

    for(int i = 0; i < water.normalmap.extents.y * water.normalmap.extents.x;
        ++i)
    {
      auto x = i % water.normalmap.extents.x;
      auto y = i / water.normalmap.extents.x;

      // If we are (0,0), for example, we should apply the sobol filter where
      // the center on the heightmap is (1,1).
      auto pt = Vec<int>{x + 1, y + 1};

      auto& dir = water.normalmap.at({x, y});
      dir.x = apply_sobel_kernel_x(pt, water.heightmap, scale);
      dir.z = apply_sobel_kernel_y(pt, water.heightmap, scale);
      dir.y = 1.0f;
      dir = glm::normalize(dir);
    }
  }

  void blit_normalmap(Texture& tex, Water& water) noexcept
  {
    auto extents = water.normalmap.extents;
    tex.allocate(water.normalmap.extents, Image_Format::Rgba);

    float* data = new float[extents.x * extents.y * 4];

    for(int i = 0; i < extents.y; ++i)
    {
      for(int j = 0; j < extents.x; ++j)
      {
        auto vec = water.normalmap.at({j, i});

        data[i * (extents.x * 4) + (j * 4) + 0] = vec.x;
        data[i * (extents.x * 4) + (j * 4) + 1] = vec.y;
        data[i * (extents.x * 4) + (j * 4) + 2] = vec.z;

        // The w component is always 0.0 because the normal is a vector, not
        // a position, obviously. It's necessary for some math done in the
        // shader.
        data[i * extents.x + (j * 4) + 3] = 0.0f;
      }
    }

    tex.blit_data(vol_from_extents(extents), data);

    delete[] data;
  }
  void blit_heightmap(Texture& tex, Water& water) noexcept
  {
    tex.allocate(water.heightmap.extents, Image_Format::Depth);
    auto vol = vol_from_extents(water.heightmap.extents);
    tex.blit_data(vol, water.heightmap.values);
  }

  template <class T, class F>
  static void write_png(std::string fn, Value_Map<T>& map, F fmt) noexcept
  {
    // All this code is pretty much based completely on the libpng manual so
    // check that out.

    std::FILE* fp = std::fopen(fn.data(), "wb");
    if(!fp)
    {
      log_w("Failed to open file '%'", fn);
      return;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  NULL, NULL, NULL);
    if(!png_ptr)
    {
      log_w("Failed to initialize libpng to write file '%'", fn);
      return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
      png_destroy_write_struct(&png_ptr, NULL);
      log_w("Failed to initialize png info to write '%'", fn);
      return;
    }

    if(setjmp(png_jmpbuf(png_ptr)))
    {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      fclose(fp);

      log_w("libpng error for '%'", fn);
      return;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, map.extents.x, map.extents.y, 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    std::vector<uint8_t*> ptrs;
    for(int i = 0; i < map.extents.y; ++i)
    {
      ptrs.push_back(new uint8_t[map.extents.x * 3]);
      for(int j = 0; j < map.extents.x; ++j)
      {
        auto color = fmt(map.at({i, j}));
        ptrs.back()[j * 3] = color.r * 0xff;
        ptrs.back()[j * 3 + 1] = color.g * 0xff;
        ptrs.back()[j * 3 + 2] = color.b * 0xff;
      }
    }

    png_set_rows(png_ptr, info_ptr, &ptrs[0]);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    for(auto* ptr : ptrs) delete ptr;

    png_destroy_write_struct(&png_ptr, &info_ptr);
  }

  struct Heightmap_Color
  {
    glm::vec3 operator()(float height) noexcept
    {
      return glm::vec3(height);
    }
  };

  void write_heightmap_png(std::string fn, Water& water) noexcept
  {
    write_png(fn, water.heightmap, Heightmap_Color{});
  }

  struct Normalmap_Color
  {
    glm::vec3 operator()(glm::vec3 const& normal) noexcept
    {
      return normal;
    }
  };

  void write_normalmap_png(std::string fn, Water& water) noexcept
  {
    write_png(fn, water.normalmap, Normalmap_Color{});
  }
} }

/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <string>
#include <cstdint>

extern "C"
{
  #include "open-simplex-noise.h"
}

#include "../common/value_map.h"
namespace redc { namespace gen
{
  struct Noise_Raii
  {
    Noise_Raii(int64_t seed, osn_context** ptr) noexcept;
    ~Noise_Raii() noexcept;

    bool allocated() const noexcept { return allocated_; }
  private:
    bool allocated_;
    osn_context* ptr_;
  };

  enum class Cell_Type
  {
    Water, Land
  };

  enum class Cell_Contents
  {
    None, Tree
  };

  struct Map_Cell
  {
    Cell_Type type = Cell_Type::Land;
    Cell_Contents contents = Cell_Contents::None;
  };
  using Grid_Map = Value_Map<Map_Cell>;

  // Forward declare this for the Other_Landmass.
  struct Terrain_Params;

  struct Terrain_Algorithm
  {
    virtual ~Terrain_Algorithm() noexcept {}
    virtual void gen(Grid_Map&) noexcept = 0;

    // Should be set before the call to gen.
    int64_t seed;
  };

  struct Terrain_Params
  {
    // Initialize the seed of each algorithm with this seed.
    int64_t seed;

    std::unique_ptr<Terrain_Algorithm> landmass_gen;

    std::unique_ptr<Terrain_Algorithm> natural_gen;
  };

  void terrain_v1_map(Grid_Map& map, Terrain_Params const&) noexcept;

  void write_png_heightmap(Grid_Map const& map, std::string const&) noexcept;
} }

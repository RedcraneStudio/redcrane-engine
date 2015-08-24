/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include <string>
#include <cstdint>

extern "C"
{
  #include "open-simplex-noise.h"
}

#include "../common/value_map.h"
namespace game { namespace strat
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

  struct Map_Cell
  {
    Cell_Type type = Cell_Type::Land;
  };
  using Grid_Map = Value_Map<Map_Cell>;

  // Forward declare this for the Other_Landmass.
  struct Terrain_Params;

  enum class Landmass_Algorithm
  {
    Radial, Other
  };
  struct Radial_Landmass
  {
    float amplitude;
    float frequency;

    // Multiplied by amplitude after each octave
    float persistence;

    // Multiplied by frequency after each octave
    float lacunarity;

    int octaves;
  };
  struct Other_Landmass
  {
    void (*gen_fn)(Grid_Map&, Terrain_Params const&) noexcept;
    void* user_data;
  };
  struct Terrain_Params
  {
    int64_t seed;

    Vec<int> origin;
    float size;

    Landmass_Algorithm algorithm;
    union
    {
      Radial_Landmass radial;
      Other_Landmass other;
    };
  };

  void terrain_v1_map(Grid_Map& map, Terrain_Params const&) noexcept;

  void write_png_heightmap(Grid_Map const& map, std::string const&) noexcept;
} }

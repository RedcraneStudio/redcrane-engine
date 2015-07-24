/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include "stratlib/event.h"

constexpr char const* const RESET_C = "\x1b[0m";
constexpr char const* const RED_C = "\x1b[95m";
constexpr char const* const CLEAR_C = "\x1b[2J";

int main(int argc, char** argv) noexcept
{
  using namespace game;
  namespace chrono = std::chrono;

  auto const map_size = Vec<int>{35,35};

  auto prng = std::mt19937(std::random_device{}());
  auto dist = std::uniform_real_distribution<float>{0.5f, 2.5f};

  // Initialize, allocate, and populate the cost map.
  strat::Cost_Map cm; cm.allocate(map_size);
  for(int i = 0; i < cm.extents.y; ++i)
  {
    for(int j = 0; j < cm.extents.x; ++j)
    {
      // One second per square.
      cm.at({j,i}) = dist(prng);
    }
  }

  strat::Event_Map event_map;
  event_map.visited_map.allocate(map_size);
  event_map.event_timer_map.allocate(map_size);

  event_map.event_timer_map.at({0,0}).active = true;

  strat::Value_Map<char> cmap;
  cmap.allocate(map_size);

  using clock_t = chrono::high_resolution_clock;

  auto now = clock_t::now();
  auto before = now;

  // Keep doing shit for 5 seconds.
  while(true)
  {
    // How much time has elapsed.
    before = now;
    now = clock_t::now();

    auto dt = chrono::duration_cast<chrono::milliseconds>(now - before)
      .count() / 1000.0f;

    std::cout << RED_C << dt << RESET_C << std::endl;

    spread(cm, event_map, dt);

    // Set some cells to pound signs.
    for(int i = 0; i < cmap.extents.y; ++i)
    {
      for(int j = 0; j < cmap.extents.x; ++j)
      {
        cmap.at({j,i}) = event_map.visited_map.at({j,i}) ? '#' : ' ';
      }
    }

    // Print the grid
    for(int i = 0; i < cmap.extents.y; ++i)
    {
      for(int j = 0; j < cmap.extents.x; ++j)
      {
        std::cout << cmap.at({i,j});
      }
      std::cout << std::endl;
    }
    std::cout << CLEAR_C;

    // Sleep for a bit then grab the time.
    std::this_thread::sleep_for(chrono::milliseconds(200));
  }

  return 0;
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <fstream>
#include <string>
#include <sstream>

#include "stratlib/event.h"

constexpr char const* const RESET_C = "\x1b[0m";
constexpr char const* const RED_C = "\x1b[95m";
constexpr char const* const CLEAR_C = "\x1b[2J";

int main(int argc, char** argv) noexcept
{
  using namespace game;
  namespace chrono = std::chrono;

  auto map_size = Vec<int>{35,35};
  strat::Cost_Map cm;
  if(argc > 1)
  {
    map_size = {0,0};

    std::vector<std::vector<float> > value_vector;

    // Load a csv with costs.
    std::ifstream file(argv[1]);
    std::string line;
    while(!std::getline(file, line, '\n').eof())
    {
      value_vector.emplace_back();

      std::istringstream line_stream{line};
      std::string value;
      while(!std::getline(line_stream, value, ',').eof())
      {
        //if(value.empty()) continue;
        value_vector.back().push_back(std::stof(value));
      }
    }

    map_size = {value_vector.front().size(), value_vector.size()};

    cm.allocate(map_size);
    for(int i = 0; i < cm.extents.y; ++i)
    {
      for(int j = 0; j < cm.extents.x; ++j)
      {
        cm.at({j,i}) = value_vector[i][j];
      }
    }
  }
  else
  {
    auto prng = std::mt19937(std::random_device{}());
    auto dist = std::uniform_real_distribution<float>{0.0f, .9f};

    // Initialize, allocate, and populate the cost map.
    cm.allocate(map_size);
    for(int i = 0; i < cm.extents.y; ++i)
    {
      for(int j = 0; j < cm.extents.x; ++j)
      {
        // One second per square.
        cm.at({j,i}) = dist(prng);
      }
    }
  }

  strat::Event_Map event_map{map_size};

  // Could this be part of the interface of Event_Map?
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

    if(spread(cm, event_map, dt))
    {
      spread(cm, event_map, 0.0f);
    }

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
        std::cout << cmap.at({j,i});
      }
      std::cout << std::endl;
    }
    //std::cout << CLEAR_C;

    // Sleep for a bit then grab the time.
    std::this_thread::sleep_for(chrono::milliseconds(200));
  }

  return 0;
}

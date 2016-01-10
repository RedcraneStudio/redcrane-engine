/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../gfx/idriver.h"
#include "../gfx/camera.h"
#include <boost/program_options.hpp>
namespace redc
{
  namespace po = boost::program_options;

  namespace gfx
  {
    struct Effect
    {
      virtual ~Effect() noexcept {}

      virtual void init(IDriver& driver,
                        po::variables_map const& vm) noexcept = 0;
      virtual void render(IDriver& driver, gfx::Camera const& cam) noexcept = 0;
    };
  }
}

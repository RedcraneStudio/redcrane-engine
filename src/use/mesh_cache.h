/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "mesh.h"
namespace redc { namespace gfx
{
  /*!
   * \brief Wraps mesh creation without a need to pass the driver.
   *
   * The driver is really what is cached, so that it doesn't have to be
   * liberally passed around everywhere like things have been.
   */
  struct Mesh_Cache
  {
    Mesh_Cache(IDriver& driver) noexcept : driver_(&driver) {}

    Mesh_Chunk load_owned_chunk(std::string filename) noexcept;
  private:
    IDriver* driver_;
  };
} }

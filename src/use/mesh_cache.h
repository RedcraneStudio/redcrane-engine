/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../assets/load.h"
namespace redc { namespace gfx
{
  /*!
   * \brief Wraps mesh creation without a need to pass the driver.
   *
   * The driver is really what is cached, so that it doesn't have to be
   * liberally passed around everywhere like things have been.
   */
  struct Mesh_Cache : public assets::Fs_Cache<Indexed_Mesh_Data>
  {
    Mesh_Cache(assets::fs::path source, assets::fs::path cache);
  private:
    Indexed_Mesh_Data load_from_source_stream(std::istream& st) override;
    Indexed_Mesh_Data load_from_cache_stream(std::istream& st) override;
    void write_cache(Indexed_Mesh_Data const& msh, std::ostream& st) override;
  };
} }

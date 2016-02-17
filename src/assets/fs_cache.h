/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "../common/debugging.h"
#include "../gfx/mesh_data.h"
#include <boost/filesystem.hpp>
namespace redc { namespace assets
{
  namespace fs = boost::filesystem;

  struct File_Desc
  {
    fs::path dir;
    std::string ext;
    bool load_bin;
  };

  /*!
   * \brief Uses a file cache to speed up loading a given asset type.
   */
  template <class T>
  struct Fs_Cache
  {
    /*!
     * \param source_fd Information about the source file type.
     * \param cache_fd Information about the cache file type.
     */
    Fs_Cache(File_Desc source_fd, File_Desc cache_fd);

    virtual ~Fs_Cache() {}

    // Loading from the cache gives the client code a stream to read from.
    T load(std::string filename);
  private:
    virtual T load_from_source_stream(std::istream& st) = 0;
    virtual T load_from_cache_stream(std::istream& st) = 0;
    virtual void write_cache(T const& t, std::ostream& fp) = 0;

    File_Desc source_fd_;
    File_Desc cache_fd_;
  };

  template <class T>
  Fs_Cache<T>::Fs_Cache(File_Desc source_fd, File_Desc cache_fd)
                  : source_fd_(source_fd), cache_fd_(cache_fd)
  {
    create_directory(cache_fd_.dir);
  }

  template <class T>
  T Fs_Cache<T>::load(std::string filename)
  {
    REDC_ASSERT_MSG(!filename.empty(), "Cannot load file with no name");

    // Find the two places this file may be
    auto cache_path = cache_fd_.dir / fs::path(filename + "." + cache_fd_.ext);

    // The source file too
    auto source_path = source_fd_.dir / fs::path(filename+"."+source_fd_.ext);

    // If our source file doesn't exist it's a bug!
    REDC_ASSERT_MSG(exists(source_path), "Source / asset file % doesn't exist",
                    source_path.native());

    // If the cache file exists and is newer than the source load it and
    // delegate to our implementation
    if(exists(cache_path) &&
       last_write_time(source_path) < last_write_time(cache_path))
    {
      auto flags = std::ios_base::in;
      if(cache_fd_.load_bin) flags |= std::ios_base::binary;

      auto stream = std::ifstream(cache_path.native(), flags);
      return load_from_cache_stream(stream);
    }

    // Otherwise we need to load the source and rewrite to the cache
    // Load the source
    auto flags = std::ios_base::in;
    if(source_fd_.load_bin) flags |= std::ios_base::binary;
    auto stream = std::ifstream(source_path.native(), flags);
    auto t = load_from_source_stream(stream);

    // Tell the implementation that they should write to the cache now
    {
      // Make sure a directory for cache path exists.
      fs::create_directories(cache_path.parent_path());

      // Open in binary mode?
      auto flags = std::ios_base::out;
      if(cache_fd_.load_bin) flags |= std::ios_base::binary;
      auto cache_stream = std::ofstream(cache_path.native(), flags);
      write_cache(t, cache_stream);
    }
    return t;
  }
} }

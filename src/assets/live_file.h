/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file is released under the 3-clause BSD License. The full license text
 * can be found in LICENSE in the top-level directory.
 */
#include <mutex>
#include <thread>
#include <string>
#include <cstdlib>
#include <fstream>
#include <uv.h>
namespace redc
{
  struct Live_File
  {
    explicit Live_File(std::string const& filename);

    Live_File(Live_File&) = default;
    Live_File& operator=(Live_File&&) = default;

    Live_File(Live_File&&) = delete;
    Live_File& operator=(Live_File const&) = delete;

    // Has the file changed since we last opened it?
    bool changed_on_disk() const;

    // Atomically open the stream and reset updated the flag.
    std::ifstream open_ifstream();

    // Return the filename of the watched file.
    std::string filename() const { return filename_; }
  private:
    const std::string filename_;

    mutable uv_loop_t loop_;
    uv_fs_poll_t handle_;

    mutable std::mutex lock_;
    bool changed_on_disk_;

    friend void poll_cb(uv_fs_poll_t* handle,
                        int status,
                        uv_stat_t const* prev,
                        uv_stat_t const* next);
  };
}

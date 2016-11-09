/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file is released under the 3-clause BSD License. The full license text
 * can be found in LICENSE in the top-level directory.
 */
#include "live_file.h"
#include "../common/log.h"
namespace redc
{
  inline bool operator==(uv_timespec_t lhs, uv_timespec_t rhs)
  {
    return lhs.tv_sec == rhs.tv_sec && lhs.tv_nsec == rhs.tv_nsec;
  }
  inline bool operator!=(uv_timespec_t lhs, uv_timespec_t rhs)
  {
    return !(lhs == rhs);
  }

  void poll_cb(uv_fs_poll_t* handle, int status, uv_stat_t const* prev,
               uv_stat_t const* next)
  {
    auto live_file = (Live_File*) handle->data;

    if(!live_file)
    {
      // We should never be here if we don't have a reference to a live file.
      log_w("I'm polling a file but I don't know what it is!");
      return;
    }
    if(status)
    {
      // The file might not exist, or something.
      // TODO: Print status code, or something.
      log_e("Something went wrong polling %", live_file->filename());
      return;
    }

    if(prev->st_mtim != next->st_mtim)
    {
      // The file changed
      std::lock_guard<std::mutex> raii_lock(live_file->lock_);
      live_file->changed_on_disk_ = true;
    }
  }

  Live_File::Live_File(std::string const& filename) : filename_(filename)
  {
    uv_loop_init(&loop_);
    uv_fs_poll_init(&loop_, &handle_);

    handle_.data = this;
    uv_fs_poll_start(&handle_, poll_cb, filename_.c_str(), 1000);
  }

  bool Live_File::changed_on_disk() const
  {
    uv_run(&loop_, UV_RUN_DEFAULT);
    return changed_on_disk_;
  }
  std::ifstream Live_File::open_ifstream()
  {
    std::lock_guard<std::mutex> raii_lock(lock_);

    if(changed_on_disk_)
    {
      // Reloading the file
      log_d("Reloading '%' from disk", filename_);
    }

    changed_on_disk_ = false;
    std::ifstream file(filename_);

    return file;
  }

}

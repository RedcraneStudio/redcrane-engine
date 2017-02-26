/*
 * Copyright (C) 2017 Luke San Antonio
 * All rights reserved.
 */

#include <algorithm>
#include "timed_text.h"

namespace redc
{
  Timed_Text_Stream::Timed_Text_Stream(float timeout) : timeout_(timeout) {}

  void Timed_Text_Stream::push_line(std::string text)
  {
    // Add a newline to our text automagically.
    // TODO: Rethink adding the newline. Should it be done here?
    text += "\n";

    // Mark a new segment.
    auto segment = Timed_Text_Segment{text.size()};

    // Add the text
    text_.append(text);

    // Record the segment.
    segments_.push_back(segment);

    for_each_observer_([&](auto observer) {
      // New text!
      observer->on_text(text);
    });
  }

  void Timed_Text_Stream::step(float dt)
  {
    // We make the assumption that the first segments in the list will also be
    // the first out, so just count how many need to be removed
    std::size_t remove_count = 0;

    // Accumulate time out of the total timeout until we reach the timeout
    for(auto& segment : segments_)
    {
      segment.accum_time += dt / timeout_;
      // When we hit one that means we've had timeout time.
      if(segment.accum_time >= 1.0f)
      {
        // This segment has lived long enough
        for_each_observer_([&](auto observer) {
          observer->on_remove(segment);
        });

        // Now actually remove it from our repr.
        text_.erase(0, segment.len);

        // Remove the segment
        ++remove_count;
      }
    }

    // Remove the segments that timed out.
    segments_.erase(segments_.begin(), segments_.begin() + remove_count);
  }

  void Timed_Text_Stream::subscribe(std::shared_ptr<Timed_Text_Observer> obs)
  {
    // Add the observer to our list.
    observers_.push_back(std::weak_ptr<Timed_Text_Observer>{obs});
  }

  std::string Timed_Text_Stream::full_text() const
  {
    return text_;
  }

  void Timed_Text_Stream::for_each_observer_(
          std::function<void (std::shared_ptr<Timed_Text_Observer>)> cb
  )
  {
    bool needs_cleanup = false;
    for(auto obs : observers_)
    {
      // Lock our observer.
      auto obs_ptr = obs.lock();
      if(obs_ptr)
      {
        // Let the callback do something with it
        cb(obs_ptr);
      }
      else
      {
        // We should get rid of bad observers
        needs_cleanup = true;
      }
    }

    if(needs_cleanup)
    {
      std::remove_if(observers_.begin(), observers_.end(), [](auto ptr){
        // If the pointer isn't valid, remove it!
        return !ptr.lock().get();

      });
    }
  }
}

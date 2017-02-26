/*
 * Copyright (C) 2017 Luke San Antonio
 * All rights reserved.
 *
 * This file implements an ongoing stream of text that should eventually
 * disappear, fade-out, etc. This can be used for chat windows, in-game
 * dialogue, etc.
 *
 * The implementation supports a polling interface and an observer interface
 * for watching changes. This is basically just the glue code so that the
 * timing is managed currently. Rendering and transitions are left to client
 * code.
 */

#ifndef RED_CRANE_ENGINE_TIMED_TEXT_H
#define RED_CRANE_ENGINE_TIMED_TEXT_H

#include <string>
#include <memory>
#include <vector>
#include "color.h"

namespace redc
{
  struct Timed_Text_Segment
  {
    Timed_Text_Segment() : Timed_Text_Segment(0) {}
    Timed_Text_Segment(std::size_t len) : len(len), accum_time(0.0f) {}

    std::size_t len;
    double accum_time;
  };

  struct Timed_Text_Observer
  {
    virtual ~Timed_Text_Observer() {}

    // We're dealing with an ad-hoc circular buffer.
    virtual void on_text(std::string text) = 0;
    virtual void on_remove(Timed_Text_Segment segment) = 0;
  };

  struct Timed_Text_Stream
  {
    // Right now we only support one timeout for all text.
    explicit Timed_Text_Stream(float default_timeout = 1.0f);

    void push_line(std::string text);

    void subscribe(std::shared_ptr<Timed_Text_Observer> dialogue);

    void step(float dt);

    std::string full_text() const;

  private:
    float timeout_;

    std::string text_;
    std::vector<Timed_Text_Segment> segments_;

    void for_each_observer_(
            std::function<void (std::shared_ptr<Timed_Text_Observer>)> cb
    );
    std::vector<std::weak_ptr<Timed_Text_Observer> > observers_;
  };
}

#endif //RED_CRANE_ENGINE_TIMED_TEXT_H

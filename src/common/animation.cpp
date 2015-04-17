/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "animation.h"
namespace game
{
  // TODO: ***TEST THIS CLASS***
  Animation::Animation(int segment_size, int segments,
                       Anim_Repeat_Mode repeat,
                       segment_fn_t segment_fn,
                       substep_fn_t substep_fn) noexcept
                       : segment_size_(segment_size + 1),
                         segments_(segments),
                         repeat_(repeat),
                         segment_fn_(segment_fn),
                         substep_fn_(substep_fn) {}

  void Animation::step() noexcept
  {
    if(done()) return;

    int segment = cur_ / segment_size_;
    int substep = cur_ % segment_size_;

    // When our substep is 0 it means we just started a new segment.
    if(substep == 0)
    {
      if(segment_fn_) segment_fn_(segment);
    }
    // When it is 1 or above it means it is our first and subsequent steps into
    // the segment respectively.
    else if(substep != 0)
    {
      if(substep_fn_) substep_fn_(substep - 1);
    }

    // Do we have to go in reverse or start over or neither?
    // Note the last case is covered by the done() conditional at the beginning
    // of this function.
    if(repeat_ == Anim_Repeat_Mode::Begin_Repeat && at_endpoint_())
    {
      // Repeat ourselves from the beginning.
      cur_ = 0;
    }
    else if(repeat_ == Anim_Repeat_Mode::Reverse_Repeat && at_endpoint_())
    {
      going_reverse_ = !going_reverse_;
      incremement_();
    }
    else
    {
      incremement_();
    }
  }
  bool Animation::done() const noexcept
  {
    return cur_ >= segments_ * segment_size_ &&
           repeat_ == Anim_Repeat_Mode::No_Repeat;
  }

  void Animation::reset() noexcept
  {
    cur_ = 0;
    going_reverse_ = false;
  }
  void Animation::reset(int segment_size, int segments,
                        Anim_Repeat_Mode repeat) noexcept
  {
    reset();

    segment_size_ = segment_size;
    segments_ = segments;
    repeat_ = repeat;
  }
  bool Animation::at_endpoint_() const noexcept
  {
    // Endpoints are checked inclusively, while the animation cur_ loops to
    // [0, segments_ * segment_size_).
    if(going_reverse_) return cur_ == 0;
    return cur_ == (segments_ * segment_size_) - 1;
  }
  void Animation::incremement_() noexcept
  {
    if(going_reverse_) --cur_;
    else ++cur_;
  }
}

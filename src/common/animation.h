/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <functional>
namespace redc
{
  enum Anim_Repeat_Mode
  {
    No_Repeat,
    Begin_Repeat,
    Reverse_Repeat
  };

  /*!
   * If the segment size is 5 and the amount of segments is 2:
   * **Start of segment 1**
   * segment_fn(0)

   * substep_fn(0)
   * substep_fn(1)
   * substep_fn(2)
   * substep_fn(3)
   * substep_fn(4)

   * **Start of segment 2**
   * segment_fn(1)

   * substep_fn(0)
   * substep_fn(1)
   * substep_fn(2)
   * substep_fn(3)
   * substep_fn(4)
   */
  struct Animation
  {
    using segment_fn_t = std::function<void (int)>;
    using substep_fn_t = segment_fn_t;

    Animation(int segment_size, int segments,
              Anim_Repeat_Mode repeat = Anim_Repeat_Mode::No_Repeat,
              segment_fn_t segment_fn = nullptr,
              substep_fn_t substep_fn = nullptr) noexcept;

    inline void set_segment_fn(segment_fn_t segment_fn) noexcept;
    inline void set_substep_fn(substep_fn_t substep_fn) noexcept;

    inline int segment_size() const noexcept;
    inline int segments() const noexcept;
    inline Anim_Repeat_Mode repeat_mode() const noexcept;

    void step() noexcept;

    bool done() const noexcept;

    void reset() noexcept;
    void reset(int segment_size, int segments,
               Anim_Repeat_Mode repeat) noexcept;

  private:
    int cur_ = 0;

    bool going_reverse_ = false;

    bool at_endpoint_() const noexcept;
    void incremement_() noexcept;

    int segment_size_;
    int segments_;
    Anim_Repeat_Mode repeat_;
    segment_fn_t segment_fn_;
    substep_fn_t substep_fn_;
  };

  inline void Animation::set_segment_fn(segment_fn_t segment_fn) noexcept
  {
    segment_fn_ = segment_fn;;
  }
  inline void Animation::set_substep_fn(substep_fn_t substep_fn) noexcept
  {
    substep_fn_ = substep_fn;
  }

  inline int Animation::segment_size() const noexcept
  {
    return segment_size_;
  }
  inline int Animation::segments() const noexcept
  {
    return segments_;
  }
  inline Anim_Repeat_Mode Animation::repeat_mode() const noexcept
  {
    return repeat_;
  }
}

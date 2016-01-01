/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <chrono>
namespace redc
{
  template <class Clock_Type = std::chrono::high_resolution_clock>
  class Timer
  {
  public:
    template <class DurationType>
    bool has_been(DurationType duration) const;

    template <class DurationType>
    DurationType has_been() const;

    void reset();
  private:
    std::chrono::time_point<Clock_Type> time_before_ = Clock_Type::now();
  };

  template <class Clock_Type>
  template <class Duration_Type>
  bool Timer<Clock_Type>::has_been(Duration_Type duration) const
  {
    return Clock_Type::now() - this->time_before_ >= duration;
  }

  template <class Clock_Type>
  template <class Duration_Type>
  Duration_Type Timer<Clock_Type>::has_been() const
  {
    using std::chrono::duration_cast;
    return duration_cast<Duration_Type>(Clock_Type::now()-this->time_before_);
  }

  template <class Clock_Type>
  void Timer<Clock_Type>::reset()
  {
    this->time_before_ = Clock_Type::now();
  }
}

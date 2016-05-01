/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <queue>
namespace redc
{
  template <class T>
  struct Event_Sink
  {
    using event_t = T;

    virtual ~Event_Sink() {}
    virtual void process_event(event_t const& event) = 0;
  };

  template <class T>
  struct Queue_Event_Source
  {
    using event_t = T;

    bool poll_event(event_t& event);

    void push_outgoing_event(event_t event);
  private:
    // Hide the implementation because we may want to add some sort of locking
    // later.
    std::queue<event_t> event_queue_;
  };

  template <class T>
  bool Queue_Event_Source<T>::poll_event(event_t& event)
  {
    if(event_queue_.size())
    {
      event = event_queue_.front();
      event_queue_.pop();
      return true;
    }
    return false;
  }
  template <class T>
  void Queue_Event_Source<T>::push_outgoing_event(event_t event)
  {
    // TODO: Prevent possible race conditions if necessary.

    event_queue_.push(std::move(event));
  }
}

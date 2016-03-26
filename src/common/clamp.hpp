/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
namespace redc
{
  template<class T>
  T clamp(T lower_bound, T upper_bound, T val)
  {
    if(val < lower_bound) { return lower_bound; }
    else if(upper_bound < val) { return upper_bound; }
    else { return val; }
  }
}

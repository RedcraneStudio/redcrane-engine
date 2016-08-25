/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "imesh.h"
#include <set>
#include <functional>
#include <algorithm>
namespace redc { namespace gfx
{
  Attribute::Attribute() : size(0), format(Data_Type::Float) {}

  bool IMesh::is_compatible(VS_Interface const& in_vs)
  {
    std::size_t in_size = in_vs.attributes.size();
    std::size_t our_size = vs_.attributes.size();
    // If the client is requesting more attributes then we have we are
    // definitely incompatible
    if(in_size > our_size)
    {
      // Get outta here
      return false;
    }

    // The other way around may work:

    std::size_t common_size = std::min(in_size, our_size);
    for(std::size_t i = 0; i < common_size; ++i)
    {
      if(vs_.attributes[i] != in_vs.attributes[i])
      {
        // Incompatible!
        return false;
      }
    }
    // If we got this far they are compatible. We purposely ignore any
    // additional parameters on our side, because we can always disable those.
    return true;
  }
  void IMesh::set_attribute(std::size_t offset, unsigned short size,
                            Data_Type format)
  {
    // Add room if necessary
    if(vs_.attributes.size() <= offset)
    {
      vs_.attributes.resize(offset+1);
    }

    // Initialize size and format
    vs_.attributes[offset].size = size;
    vs_.attributes[offset].format = format;
  }
} }

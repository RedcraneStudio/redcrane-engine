/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "structure.h"
namespace game
{
  Structure_Instance::
  Structure_Instance(IStructure& s, Orient o) noexcept
    : structure_type(&s),
      obj(structure_type->make_obj()),
      orientation(o) {}
  Structure_Instance::Structure_Instance(Structure_Instance const& s) noexcept
    : structure_type(s.structure_type),
      obj(share_object_keep_ownership(s.obj)),
      orientation(s.orientation) {}
  Structure_Instance& Structure_Instance::
  operator=(Structure_Instance const& i) noexcept
  {
    structure_type = i.structure_type;
    obj = share_object_keep_ownership(i.obj);
    orientation = i.orientation;
    return *this;
  }
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"

#include "../gfx/support/mesh_conversion.h"
#include "../gfx/support/load_wavefront.h"
#include "../gfx/support/allocate.h"
#include "../gfx/support/format.h"
#include "../gfx/support/write_data_to_mesh.h"

namespace redc { namespace gfx
{
  Mesh_Result load_mesh(IDriver& d, Mesh_Load_Params const& params) noexcept
  {
    auto split_data = load_wavefront(params.filename);
    auto indexed = to_indexed_mesh_data(split_data);

    auto mesh_repr = d.make_mesh_repr();
    allocate_standard_mesh_buffers(indexed.vertices.size(),
                                   indexed.elements.size(),
                                   *mesh_repr,
                                   Usage_Hint::Draw,
                                   Upload_Hint::Static);
    format_standard_mesh_buffers(*mesh_repr);

    Mesh_Result ret;
    ret.chunk = write_data_to_mesh(indexed, std::move(mesh_repr), 0, 0);

    if(params.retain_mesh) ret.data = indexed;
    return ret;
  }
} }

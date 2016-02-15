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

#include "mesh_cache.h"

namespace redc { namespace gfx
{
  Mesh_Result package_mesh(IDriver& d, Indexed_Mesh_Data&& data, bool keep_msh)
  {
    auto mesh_repr = d.make_mesh_repr();
    allocate_standard_mesh_buffers(data.vertices.size(),
                                   data.elements.size(),
                                   *mesh_repr,
                                   Usage_Hint::Draw,
                                   Upload_Hint::Static);
    format_standard_mesh_buffers(*mesh_repr);

    Mesh_Result ret;
    ret.chunk = write_data_to_mesh(data, std::move(mesh_repr), 0, 0);

    if(keep_msh) ret.data = std::move(data);
    return ret;
  }

  Mesh_Result load_mesh(IDriver& d, Mesh_Load_Params const& params)
  {
    auto split_data = load_wavefront(params.filename);
    auto indexed = to_indexed_mesh_data(split_data);

    return package_mesh(d, std::move(indexed), params.retain_mesh);
  }

  Mesh_Result load_mesh(IDriver& d, Mesh_Cache& mc,
                        Mesh_Load_Params const& params)
  {
    // Use the cache to load!
    auto mesh = mc.load(params.filename);
    // Package!
    return package_mesh(d, std::move(mesh), params.retain_mesh);
  }

  Mesh_Chunk load_chunk(IDriver& driver, Mesh_Cache& mc, std::string const& f)
  {
    auto res = load_mesh(driver, mc, {f, false});
    return copy_mesh_chunk_move_mesh(res.chunk);
  }
} }

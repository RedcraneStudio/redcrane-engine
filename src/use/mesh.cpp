/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh.h"

#include "../gfx/extra/mesh_conversion.h"
#include "../gfx/extra/allocate.h"
#include "../gfx/extra/format.h"
#include "../gfx/extra/write_data_to_mesh.h"
#include "../gfx/extra/load_wavefront.h"

#include "mesh_cache.h"

namespace redc { namespace gfx
{
  Mesh_Result package_mesh(IDriver& d, Indexed_Mesh_Data&& data, bool keep_msh)
  {
    std::unique_ptr<IMesh> mesh = d.make_mesh_repr();

    // Index 0: Position
    // Index 1: Normal
    // Index 2: UV
    std::vector<std::unique_ptr<IBuffer> > buffers(4);
    d.make_buffers(buffers.size(), &buffers[0]);

    allocate_standard_mesh_buffers(data.vertices.size(),
                                   data.elements.size(),
                                   buffers,
                                   Usage_Hint::Draw,
                                   Upload_Hint::Static);
    format_standard_mesh_buffers(*mesh, buffers);

    Mesh_Result ret;
    ret.chunk = write_standard_data_to_mesh_buffers(data, std::move(mesh),
                                                    std::move(buffers), 0, 0);

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
    Mesh_Load_Params params;
    params.filename = f;
    params.retain_mesh = false;

    Mesh_Result res = load_mesh(driver, mc, params);
    return copy_mesh_chunk_move_mesh(res.chunk);
  }
} }

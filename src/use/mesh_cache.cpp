/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh_cache.h"
#include "../common/debugging.h"
#include "../gfx/support/load_wavefront.h"
#include "../gfx/support/mesh_conversion.h"
#include "../gfx/support/allocate.h"
#include "../gfx/support/format.h"
#include "../gfx/support/write_data_to_mesh.h"

namespace redc { namespace gfx
{
  Mesh_Cache::Mesh_Cache(assets::fs::path source_path,
                         assets::fs::path cache_dir)
                         : Fs_Cache{{source_path, "obj", false},
                                    {cache_dir, "obj.cache", true}} {}

  Indexed_Mesh_Data Mesh_Cache::load_from_source_stream(std::istream& st)
  {
    // Load .obj file
    auto split_mesh = gfx::load_wavefront(st);
    // Return combined
    return gfx::to_indexed_mesh_data(split_mesh);
  }
  Indexed_Mesh_Data Mesh_Cache::load_from_cache_stream(std::istream& st)
  {
    // Load it in with msgpack, because that's how we wrote it!

    msgpack::unpacker unpacker;

    constexpr std::size_t read_size = 4096;
    while(!st.eof())
    {
      unpacker.reserve_buffer(read_size);

      st.read(unpacker.buffer(), read_size);
      unpacker.buffer_consumed(st.gcount());
    }

    msgpack::unpacked object;
    bool found = unpacker.next(object);
    REDC_ASSERT(found);

    Indexed_Mesh_Data mesh;
    REDC_ASSERT_NO_THROW(mesh = object.get().as<Indexed_Mesh_Data>());

    return mesh;
  }

  void Mesh_Cache::write_cache(Indexed_Mesh_Data const& msh,
                               std::ostream& str)
  {
    // Write the stuff to the file
    msgpack::pack(str, msh);
  }
} }


/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "write_data_to_mesh.h"
namespace redc { namespace gfx
{
  template <class T>
  void write_vertex_member_to_buffer(Vertex const* begin, std::size_t length,
                                     T Vertex::* member,
                                     IBuffer& buf, std::size_t byte_offset)
  {
    // If what we have allocated is *less than* what we need.
    if(buf.allocated_size() < byte_offset + sizeof(begin->*member))
    {
      // Log and just don't write to the mesh.
      log_e("Failed to write vertex data to mesh because there isn't enough"
            " space for for every vertex.");
      return;
    }

    // Make the data contigous
    std::vector<T> data;
    for( ; length; ++begin, --length)
    {
      data.push_back(begin->*member);
    }

    buf.update(byte_offset, data.size() * sizeof(T), &data[0]);
  }

  void write_vertices_to_buffer(std::vector<Vertex> const& vertices,
                                Mesh_Chunk& chunk, std::size_t element_offset)
  {
    if(chunk.buffers.size() < 3) return;

    // We're establishing a convention with this function.
    // First three buffers represent positions, normals, and texture
    // coordinates, respectively.
    IBuffer& pos_buf = *chunk.buffers[0];
    IBuffer& norm_buf = *chunk.buffers[1];
    IBuffer& uv_buf = *chunk.buffers[2];

    write_vertex_member_to_buffer(&vertices[0], vertices.size(),
                                  &Vertex::position, pos_buf,
                                  element_offset * sizeof(Vertex::position));

    write_vertex_member_to_buffer(&vertices[0], vertices.size(),
                                  &Vertex::normal, norm_buf,
                                  element_offset * sizeof(Vertex::normal));

    write_vertex_member_to_buffer(&vertices[0], vertices.size(),
                                  &Vertex::uv, uv_buf,
                                  element_offset * sizeof(Vertex::uv));

    // Partially initialize the chunk so we can render with it. If the user
    // copies the element array buffer later, that function will overrite these
    // values.
    chunk.start = element_offset;
    chunk.count = vertices.size();
  }
  void write_element_array_to_buffer(std::vector<unsigned int> const& data,
                                     Mesh_Chunk& c, std::size_t element_offset,
                                     int base_vertex)
  {
    if(c.buffers.size() < 4) return;
    // Use the fourth buffer for an element array.
    IBuffer& buf = *c.buffers[3];

    std::size_t size = data.size() * sizeof(data[0]);
    std::size_t byte_offset = element_offset * sizeof(unsigned int);

    if(buf.allocated_size() < byte_offset + size)
    {
      log_e("Failed to write element array data to the mesh because there"
            " isn't enough room for every value.");
    }

    buf.update(byte_offset, size, &data[0]);

    c.start = element_offset;
    c.count = data.size();
    c.base_vertex = base_vertex;
  }

  Mesh_Chunk initialize_chunk(Maybe_Owned<IMesh> mesh,
                              std::vector<std::unique_ptr<IBuffer> > bufs)
  {
    Mesh_Chunk ret;
    ret.mesh = std::move(mesh);

    // Move buffers to the chunk
    for(std::size_t i = 0; i < bufs.size(); ++i)
    {
      ret.buffers.push_back(std::move(bufs[i]));
    }

    return ret;
  }

  Mesh_Chunk write_standard_data_to_mesh_buffers(
    Indexed_Mesh_Data const& data, Maybe_Owned<IMesh> mesh,
    std::vector<std::unique_ptr<IBuffer> > bufs,
    std::size_t vertex_element_offset, std::size_t element_array_offset
  )
  {
    // Convention of fourth buffer being used to store element indices.
    Mesh_Chunk ret = initialize_chunk(std::move(mesh), std::move(bufs));

    write_vertices_to_buffer(data.vertices, ret, vertex_element_offset);
    write_element_array_to_buffer(data.elements, ret, element_array_offset,
                                  vertex_element_offset);
    ret.type = data.primitive;

    return ret;
  }
  Mesh_Chunk write_standard_data_to_mesh_buffers(
    Ordered_Mesh_Data const& data, Maybe_Owned<IMesh> mesh,
    std::vector<std::unique_ptr<IBuffer> > bufs,
    std::size_t vertex_element_offset
  )
  {
    Mesh_Chunk ret = initialize_chunk(std::move(mesh), std::move(bufs));

    write_vertices_to_buffer(data.vertices, ret, vertex_element_offset);
    ret.type = data.primitive;

    return ret;
  }
} }

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "write_data_to_mesh.h"
namespace game { namespace gfx
{
  void allocate_mesh_buffers(Indexed_Mesh_Data const& data,
                             Mesh& mesh, Upload_Hint up,
                             Usage_Hint us) noexcept
  {
    mesh.allocate_buffer(data.vertices.size() * sizeof(Vertex::position),
                         us,up);
    mesh.allocate_buffer(data.vertices.size() * sizeof(Vertex::normal), us,up);
    mesh.allocate_buffer(data.vertices.size() * sizeof(Vertex::uv), us, up);

    mesh.allocate_element_array(data.elements.size(), us, up);
  }
  void allocate_mesh_buffers(Ordered_Mesh_Data const& data,
                             Mesh& mesh, Upload_Hint up,
                             Usage_Hint us) noexcept
  {
    mesh.allocate_buffer(data.vertices.size() * sizeof(Vertex::position),
                         us, up);
    mesh.allocate_buffer(data.vertices.size() * sizeof(Vertex::normal),
                         us, up);
    mesh.allocate_buffer(data.vertices.size() * sizeof(Vertex::uv),
                         us, up);
  }
  void format_mesh_buffers(Mesh& m) noexcept
  {
    m.format_buffer(m.get_buffer(0), 0, 3, Buffer_Format::Float, 0, 0);
    m.enable_vertex_attrib(0);

    m.format_buffer(m.get_buffer(1), 1, 3, Buffer_Format::Float, 0, 0);
    m.enable_vertex_attrib(1);

    m.format_buffer(m.get_buffer(2), 2, 2, Buffer_Format::Float, 0, 0);
    m.enable_vertex_attrib(2);
  }

  template <class T>
  void write_vertex_member_to_mesh(Vertex const* begin, unsigned int length,
                                   T Vertex::* member,
                                   Mesh& m,
                                   unsigned int buffer,
                                   unsigned int byte_offset) noexcept
  {
    // If what we have allocated is *less than* what we need.
    if(m.get_buffer_size(buffer) < byte_offset + sizeof(begin->*member))
    {
      // Log and just don't write to the mesh.
      log_e("Failed to write vertex data to mesh because there isn't enough"
            " space for for every vertex.");
      return;
    }

    std::vector<T> data;
    for( ; length; ++begin, --length)
    {
      data.push_back(begin->*member);
    }
    m.buffer_data(buffer, byte_offset, data.size() * sizeof(T), &data[0]);
  }

  void write_vertices_to_mesh(std::vector<Vertex> const& vertices,
                              Mesh_Chunk& chunk,
                              Mesh& mesh,
                              unsigned int elemnt_offset) noexcept
  {
    // We're establishing a convention with this function.
    // First three buffers represent positions, normals, and texture
    // coordinates, respectively.
    auto pos_buf = mesh.get_buffer(0);
    auto norm_buf = mesh.get_buffer(1);
    auto uv_buf = mesh.get_buffer(2);

    write_vertex_member_to_mesh(&vertices[0], vertices.size(),
                                &Vertex::position, mesh, pos_buf,
                                elemnt_offset * sizeof(Vertex::position));

    write_vertex_member_to_mesh(&vertices[0], vertices.size(),
                                &Vertex::normal, mesh, norm_buf,
                                elemnt_offset * sizeof(Vertex::normal));

    write_vertex_member_to_mesh(&vertices[0], vertices.size(),
                                &Vertex::uv, mesh, uv_buf,
                                elemnt_offset * sizeof(Vertex::uv));


    // Partially initialize the chunk so we can render with it. If the user
    // copies the element array buffer later, that function will overrite these
    // values.
    chunk.mesh.set_pointer(&mesh);
    chunk.start = elemnt_offset;
    chunk.count = vertices.size();
  }
  void write_element_array_to_mesh(std::vector<unsigned int> const& data,
                                   Mesh_Chunk& c,
                                   Mesh& mesh,
                                   unsigned int buffer,
                                   unsigned int element_offset,
                                   int base_vertex) noexcept
  {
    // No conventions established.

    auto size = data.size() * sizeof(data[0]);

    if(mesh.get_buffer_size(buffer) < element_offset + size)
    {
      log_e("Failed to write element array data to the mesh because there"
            " isn't enough room for every value.");
    }

    mesh.buffer_data(buffer, sizeof(data[0]) * element_offset, size, &data[0]);

    c.start = element_offset;
    c.count = data.size();
    c.mesh.set_pointer(&mesh);

    c.base_vertex = base_vertex;
  }

  Mesh_Chunk write_data_to_mesh(Indexed_Mesh_Data const& data, Mesh& mesh,
                                unsigned int vertice_element_offset,
                                unsigned int element_array_offset) noexcept
  {
    // Convention of fourth buffer being used to store element indices.

    Mesh_Chunk ret;

    write_vertices_to_mesh(data.vertices, ret, mesh, vertice_element_offset);
    write_element_array_to_mesh(data.elements, ret, mesh, mesh.get_buffer(3),
                                element_array_offset, vertice_element_offset);

    ret.type = data.primitive;

    return ret;
  }
  Mesh_Chunk write_data_to_mesh(Ordered_Mesh_Data const& data, Mesh& mesh,
                                unsigned int vert_elemnt_off) noexcept
  {
    Mesh_Chunk ret;

    write_vertices_to_mesh(data.vertices, ret, mesh, vert_elemnt_off);

    ret.type = data.primitive;

    return ret;
  }
} }

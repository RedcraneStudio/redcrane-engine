/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "glad/glad.h"
#include <array>
#include "../../common/mesh.h"
namespace game { namespace gfx { namespace gl
{
  struct GL_Mesh : public Mesh
  {
    virtual ~GL_Mesh() noexcept;

    void allocate(unsigned int max_verts,
                  unsigned int max_elemnt_indices, Usage_Hint, Upload_Hint,
                  Primitive_Type) noexcept override;

    void allocate_from(Mesh_Data const&) noexcept override;

    void set_vertices(unsigned int begin,
                      unsigned int length,
                      Vertex const* data) noexcept override;
    void set_element_indices(unsigned int begin,
                             unsigned int length,
                             unsigned int const* indices) noexcept override;
    void set_num_element_indices(unsigned int) noexcept override;

    void bind() const noexcept;
    void draw() const noexcept;

    GLuint vao;
    std::array<GLuint, 4> bufs;
    int num_indices;
    GLenum primitive;

  private:
    void unallocate_() noexcept;
    void allocate_vao_() noexcept;
    void allocate_buffers_() noexcept;
    void allocate_array_buffer_(GLuint buf, GLuint attrib_index, GLint cs,
                                std::size_t size, GLenum type,
                                GLenum usage) noexcept;
    void allocate_array_buffer_(GLuint buf, GLuint attrib_index, GLint cs,
                                std::size_t size, GLenum type,
                                GLenum usage, void* data) noexcept;
  };
} } }

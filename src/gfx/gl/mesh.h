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

    void prepare(Mesh_Data const& data) noexcept override;
    void set_vertices(unsigned int b, unsigned int l,
                      Vertex const*) noexcept override;

    void bind() const noexcept;
    void draw() const noexcept;

    GLuint vao;
    std::array<GLuint, 4> bufs;
    int num_faces = 0;
    GLenum primitive;
  };
} } }

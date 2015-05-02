/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <cstring>
#include "aabb.h"
namespace game
{
  enum class Usage_Hint
  {
    Draw, Read, Copy
  };
  enum class Upload_Hint
  {
    Static, Dynamic, Stream
  };
  enum class Primitive_Type
  {
    Triangle, Line
  };

  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
  };

  struct Mesh_Data
  {
    // Everything is fair game.
    Mesh_Data() = default;
    Mesh_Data(Mesh_Data&&) noexcept = default;
    Mesh_Data(Mesh_Data const&) = default;
    Mesh_Data& operator=(Mesh_Data const&) = default;
    Mesh_Data& operator=(Mesh_Data&&) noexcept = default;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> elements;

    Usage_Hint usage_hint = Usage_Hint::Draw;
    Upload_Hint upload_hint = Upload_Hint::Dynamic;

    Primitive_Type primitive = Primitive_Type::Triangle;

    // This function doesn't strictly need to be in a class member function, it
    // can be a free function, but from a Mesh::set_vertices implementation
    // the free function would be impossible to call without a namespace
    // specifier (otherwise it would be recursive).
    inline void set_vertices(unsigned int b, unsigned int l,
                             Vertex const* v) noexcept;
  };

  // A reference implementation of Mesh::set_vertices, basically.
  inline void Mesh_Data::set_vertices(unsigned int b, unsigned int l,
                                      Vertex const* v) noexcept
  {
    std::memcpy(&vertices[b], v, l);
  }

  struct Mesh
  {
    virtual ~Mesh() noexcept {}

    /*!
     * \brief Allocates a mesh based on given data.
     *
     * If this function is called more than once, a mesh is to be reconstructed
     * possibly from scratch, unless it is possible to use existing memory.
     *
     * Only the value of specific vertex attributes may be modified after the
     * mesh is prepared. Everything else requires a complete reallocation.
     *
     * \note The representation may or may not retain the given Mesh_Data
     * structure.
     */
    virtual void prepare(Mesh_Data const& data) noexcept = 0;
    /*!
     * \brief Allocates a mesh based on given data while taking ownership of
     * the given mesh data.
     *
     * Should be used if the client of this class can give up the data,
     * otherwise it will just be copied.
     */
    inline virtual void prepare(Mesh_Data&& data) noexcept;

    /*!
     * \brief **Changes** a single vertex in the mesh without reallocating
     * everything, ideally.
     *
     * Element is an offset into the vertices field of the Mesh_Data provided
     * in a previous call to prepare.
     */
    inline void set_vertex(unsigned int elemnt, Vertex const&) noexcept;

    /*!
     * \brief Sets a sequence of vertices starting from a certain vertex in the
     * prepared mesh.
     */
    virtual void set_vertices(unsigned int begin,
                              unsigned int length,
                              Vertex const*) noexcept = 0;
  };
  void Mesh::set_vertex(unsigned int elemnt, Vertex const& v) noexcept
  {
    set_vertices(elemnt, 1, &v);
  }
  void Mesh::prepare(Mesh_Data&& data) noexcept
  {
    // Just default to making a copy.
    prepare(static_cast<Mesh_Data const&>(data));
  }

  AABB generate_aabb(Mesh_Data const& mesh) noexcept;
}

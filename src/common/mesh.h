/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <cstring>
#include "aabb.h"
#include <boost/optional.hpp>
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

  struct Mesh_Chunk
  {
    unsigned int offset;
    unsigned int count;
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

    // A reference implementation of Mesh::set_vertices, basically.
    // This function doesn't strictly need to be in a class member function, it
    // can be a free function, but from a Mesh::set_vertices implementation
    // the free function would be impossible to call without a namespace
    // specifier (otherwise it would be recursive).
    inline void set_vertices(unsigned int b, unsigned int l,
                             Vertex const* v) noexcept
    {
      std::memcpy(&vertices[b], v, l);
    }
    // A reference implementation of Mesh::set_element_indices, basically.
    inline void set_element_indices(unsigned int b, unsigned int l,
                                    unsigned int const* i) noexcept
    {
      std::memcpy(&elements[b], i, l);
    }

    Mesh_Chunk append(Mesh_Data const&) noexcept;
  };

  struct Mesh
  {
    virtual ~Mesh() noexcept {}

    /*!
     * \brief Sets a sequence of vertices starting from a certain vertex in an
     * allocated mesh.
     */
    virtual void set_vertices(unsigned int begin,
                              unsigned int length,
                              Vertex const* data) noexcept = 0;

    /*!
     * \brief Set element indices up to a maximum set during allocation.
     */
    virtual void set_element_indices(unsigned int begin,
                                     unsigned int length,
                                     unsigned int const* indices) noexcept = 0;

    virtual void set_num_element_indices(unsigned int) noexcept = 0;

    /*!
     * \brief **Changes** a single vertex in the mesh without reallocating
     * everything, ideally.
     *
     * Element is an offset into the vertices field of the Mesh_Data provided
     * in a previous call to prepare.
     */
    inline void set_vertex(unsigned int elemnt, Vertex const& v) noexcept
    { set_vertices(elemnt, 1, &v); }

    inline bool allocated() const noexcept { return allocated_; }

    void allocate(unsigned int max_verts, unsigned int max_elemnt_indices,
                  Usage_Hint, Upload_Hint, Primitive_Type) noexcept;
    void allocate_from(Mesh_Data const&) noexcept;
    void allocate_from(Mesh_Data&&) noexcept;
  private:
    bool allocated_ = false;

    /*!
     * \brief Allocates a mesh for a certain amount of vertices and element
     * indices.
     *
     * If this function is called, no mesh data is guaranteed to be retained.
     */
    virtual void allocate_(unsigned int max_verts,
                          unsigned int max_elemnt_indices,
                          Usage_Hint, Upload_Hint,
                          Primitive_Type) noexcept = 0;

    /*!
     * \brief Allocates a mesh given some initial mesh data.
     *
     * The default implementation is equivalent to allocation and then setting
     * each of the components in sequence. ie allocation, set_vertices,
     * set_element_indices, set_num_element_indices. This process may be
     * optimized in implementations.
     */
    virtual void allocate_from_(Mesh_Data const&) noexcept;
    /*!
     * \brief Allocates a mesh given some initial mesh data, the impl is
     * allowed to steal the guts if that works better for it.
     */
    virtual void allocate_from_(Mesh_Data&&) noexcept;
  };

  AABB generate_aabb(Mesh_Data const& mesh) noexcept;

  struct Vert_Ref
  {
    boost::optional<unsigned int> position;
    boost::optional<unsigned int> normal;
    boost::optional<unsigned int> tex_coord;
  };

  bool operator==(Vert_Ref const& v1, Vert_Ref const& v2) noexcept;
  bool operator<(Vert_Ref const& lhs, Vert_Ref const& rhs) noexcept;

  // Optimized in the sense that duplicate vertex references will not be
  // duplicated in the buffer.
  Mesh_Data make_optimized_mesh_data(std::vector<Vert_Ref> indices,
                                     std::vector<glm::vec3> pos,
                                     std::vector<glm::vec3> norm = {},
                                     std::vector<glm::vec2> uv = {}) noexcept;
  }

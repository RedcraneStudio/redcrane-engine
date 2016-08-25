/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 *
 * This file defines the abstract class for a mesh to be implemented by
 * drivers.
 */
#pragma once
#include <array>
#include <vector>
#include <tuple>
#include "common.h"
#include "ihandle.h"
namespace redc { namespace gfx
{
  struct Attribute
  {
    Attribute();
    unsigned short size;
    Data_Type format;
  };

  inline bool operator==(Attribute const& at1, Attribute const& at2)
  {
    return at1.size == at2.size && at1.format == at2.format;
  }
  inline bool operator!=(Attribute const& at1, Attribute const& at2)
  {
    return !(at1 == at2);
  }

  struct VS_Interface
  {
    std::vector<Attribute> attributes;
  };

  /*!
   * \brief A mesh knows how to draw itself.
   */
  struct IMesh : public IHandle
  {
    virtual ~IMesh() {}

    // type parameter must be Scalar, Vec2, Vec3, or Vec4
    virtual void format_buffer(IBuffer& buf,
                               Attrib_Bind loc,
                               Attrib_Type type,
                               Data_Type format,
                               std::size_t stride,
                               std::size_t offset) = 0;

    virtual void enable_attrib_bind(Attrib_Bind attrib) = 0;
    virtual void disable_attrib_bind(Attrib_Bind attrib) = 0;

    virtual void set_primitive_type(Primitive_Type) = 0;
    virtual Primitive_Type get_primitive_type() = 0;

    /*
     * \brief Use the given buffer as an element array when rendering with
     * draw_elements.
     */
    virtual void use_element_buffer(IBuffer& buf, Data_Type type) = 0;

    virtual void draw_arrays(unsigned int start, unsigned int c) = 0;
    /*!
     * \param st Start index; given in elements, not bytes.
     */
    virtual void draw_elements(unsigned int st, unsigned int c) = 0;
    virtual void draw_elements_base_vertex(unsigned int st, unsigned int c,
                                           unsigned int bv) = 0;

    // Compares the one passed in with the internal one kept up to date by the
    // super class.
    bool is_compatible(VS_Interface const& vs);
  private:
    void set_attribute(std::size_t offset, unsigned short size, Data_Type format);
    VS_Interface vs_;
  };
} }

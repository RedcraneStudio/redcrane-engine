/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>
namespace redc
{
  namespace fs = boost::filesystem;

  //! Represents an buffer in CPU memory.
  struct Buffer
  {
    std::string name;
    std::vector<uint8_t> buf;
  };

  enum class Buf_View_Target
  {
    Array, Element_Array
  };

  struct Buffer_View
  {
    std::string name;
    uint8_t* base_ptr;
    std::size_t size;
    Buf_View_Target target;
  };

  enum class Component_Type
  {
    Float, Short, UShort, Byte, UByte
  };
  enum class Attribute_Type
  {
    Scalar, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4
  };

  struct Accessor
  {
    std::string name;
    uint8_t* base_ptr;
    std::size_t count;
    std::size_t stride;
    Component_Type component_type;
    Attribute_Type attribute_type;

    // Min and max not considered
  };

  struct Asset
  {
    boost::optional<fs::path> filepath;
    std::unordered_map<std::string, Buffer> buffers;
    std::unordered_map<std::string, Buffer_View> buf_views;
    std::unordered_map<std::string, Accessor> accessors;
  };

  boost::optional<bool> load_gltf_file(std::string name);
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include "../common/maybe_owned.hpp"
#include "camera.h"

#include <vector>
#include <unordered_map>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include "../../gltf/tiny_gltf_loader.h"

#include "enums.h"
#include "types.h"

namespace redc
{
  enum class Buffer_Target
  {
    CPU, Array, Element_Array
  };

  struct Buffer
  {
    // Target type
    Buffer_Target target;
    // Cached (if GPU target) or actual data (if CPU buf target)
    std::vector<uint8_t> data;
    // Buffer representation
    boost::optional<Buf_Repr> repr;
  };

  // The most significant nibble is type (1 = vector, 2 = matrix) and the other
  // nibble is the amount. Will be used to quickly distinquish between vector
  // and matrix types.
  enum class Attrib_Type : unsigned char
  {
    Scalar = 0x11, Vec2 = 0x12, Vec3 = 0x13, Vec4 = 0x14, Mat2 = 0x22,
    Mat3 = 0x23, Mat4 = 0x24
  };

  using Buf_Ref = std::size_t;
  struct Accessor
  {
    // Index into the assets buffers.
    Buf_Ref buf_i;

    // Number of attributes
    std::size_t count;

    // In bytes
    std::size_t offset;
    std::size_t stride;

    // Type of each element e.g. float
    Data_Type data_type;
    // What kind of attribute e.g. 4-component vector.
    Attrib_Type attrib_type;
  };

  // Type of parameter value
  enum class Param_Type
  {
    Byte, UByte, Short, UShort, Int, UInt, Float, Vec2, Vec3, Vec4, IVec2,
    IVec3, IVec4, Bool, BVec2, BVec3, BVec4, Mat2, Mat3, Mat4, Sampler2D
  };

  // Represents a uniform value
  union Param_Value
  {
    // Signed and unsigned byte
    int8_t byte;
    uint8_t ubyte;

    // Signed and unsigned short.
    short shrt;
    unsigned short ushrt;

    // Unsigned int and sampler **index**
    unsigned int uint;

    // Float, vectors, and matrices
    std::array<float, 16> floats;
    // signed integer and ivec
    std::array<int, 4> ints;
    // bool and bvec
    std::array<bool, 4> bools;
  };

  struct Parameter
  {
    Param_Type type;
    Param_Value value;
  };

  // Represents a semantic uniform
  enum class Param_Semantic
  {
    Local, Model, View, Projection, Model_View, Model_View_Projection,
    Model_Inverse, View_Inverse, Projection_Inverse, Model_View_Inverse,
    Model_View_Projection_Inverse, Model_Inverse_Transpose,
    Model_View_Inverse_Transpose, Viewport, Joint_Matrix
  };

  // Used to represent things like NORMAL_0 or TEXCOORD_2.
  struct Attrib_Semantic
  {
    enum
    {
      Position, Normal, Texcoord, Color, Joint, Weight
    } kind;

    // This is optional because whether it was included will effect the string
    // we must use for lookup later.
    boost::optional<unsigned short> index;
  };

  inline bool operator==(Attrib_Semantic const& lhs, Attrib_Semantic const& rhs)
  {
    return lhs.kind == rhs.kind && lhs.index == rhs.index;
  }

  struct Attrib_Semantic_Hash
  {
    std::hash<uint64_t> hasher;
    std::size_t operator()(Attrib_Semantic sem) const
    {
      uint64_t num = (uint64_t) sem.index.value_or(0);
      num |= static_cast<uint64_t>(sem.kind) << 32;
      return hasher(num);
    }
  };

  template <class U, class... Args>
  using Attrib_Semantic_Map = std::unordered_map<Attrib_Semantic, U,
                                                 Attrib_Semantic_Hash, Args...>;

  // Techniques set up a lot of rendering, so this is where extensions should be
  // added if at all.

  struct Asset;

  struct Shader
  {
    Shader_Repr repr;
    Shader_Type type;
  };

  struct Program
  {
    // A program owns its representation and they are not shared with other
    // programs.
    Program_Repr repr;

    // Maps attribute names to bind locations. We look them up once at creation
    // time with glGetAttribLocation.
    std::unordered_map<std::string, Attrib_Bind> attributes;
  };

  using Program_Ref = std::size_t;

  using Node_Ref = std::size_t;
  struct Param_Decl
  {
    // Optional node to take transformation from
    boost::optional<Node_Ref> node;

    // Must be one for attributes
    int count;

    // Value and type of the parameter. I chose not to put a Parameter here and
    // instead use both a type and value explicitely because the value here is
    // optional I think, whereas type is not. This is distinct from a value
    // provided in material.values.
    Param_Type type;
    boost::optional<Param_Value> default_value;

    // Optional semantic meaning
    boost::optional<boost::variant<Param_Semantic, Attrib_Semantic> > semantic;
    // Either a parameter (uniform) or an attribute (input variable).
    boost::variant<Param_Bind, Attrib_Bind> bind;
  };

  struct Technique
  {
    ~Technique() {}

    Program_Ref program_i;

    // This includes name, type and bind information.
    std::unordered_map<std::string, Param_Decl> parameters;
  };

  using Technique_Ref = std::size_t;

  // A material uses some technique to rendering the mesh given some unique
  // parameters to that material.
  struct Material
  {
    Technique_Ref technique_i;

    // We obtained the bind location using the technique. A technique can be
    // shared between materials so worse case these parameters need to be set
    // before each draw using this material, which is why we should batch first
    // by technique then by material. Because we store a bind, we give up the
    // ability to switch the technique of a material at runtime, but that's okay
    // because we a primitive can still switch to a new material at runtime.
    std::vector<std::pair<Param_Bind, Parameter> > parameters;
  };

  // Because the material of a primitive is not going to change, we can
  // statically bind accessors to shader bind locations using information we
  // know about the material.

  using Accessor_Ref = std::size_t;
  using Material_Ref = std::size_t;
  struct Primitive
  {
    // Maps semantic attributes to accessors. We don't use binds because that
    // would make it impossible to switch techniques.
    Attrib_Semantic_Map<Accessor_Ref> attributes;
    boost::optional<Accessor_Ref> indices;
    Material_Ref mat_i;
    Render_Mode mode;
  };

  struct Mesh
  {
    // A mesh will own its representation so that in the best case (only one set
    // of primitives) we don't have to constantly reformat the vertex array
    // object.
    Mesh_Repr repr;

    // A mesh does own its own primitives though.
    std::vector<Primitive> primitives;
  };

  using Mesh_Ref = std::size_t;
  struct Node
  {
    std::vector<Mesh_Ref> meshes;

    std::vector<Node_Ref> children;
    boost::optional<Node_Ref> parent;

    boost::optional<std::array<float, 4> > rotation;
    boost::optional<std::array<float, 3> > scale;
    boost::optional<std::array<float, 3> > translation;
    boost::optional<std::array<float, 16> > matrix;
  };

  struct Asset
  {
    Asset() = default;
    ~Asset();

    Asset(Asset&&) = default;
    Asset& operator=(Asset&&) = default;

    std::vector<Buffer> buffers;
    std::vector<Texture_Repr> textures;

    std::vector<Accessor> accessors;

    std::vector<Shader> shaders;
    std::vector<Program> programs;
    std::vector<Technique> techniques;

    std::vector<Material> materials;

    std::vector<Mesh> meshes;
    std::vector<Node> nodes;

    std::vector<std::string> buf_names;
    std::vector<std::string> texture_names;

    std::vector<std::string> accessor_names;

    std::vector<std::string> shader_names;
    std::vector<std::string> program_names;
    std::vector<std::string> technique_names;

    std::vector<std::string> material_names;

    std::vector<std::string> mesh_names;
    std::vector<std::string> node_names;
  };

  Asset load_asset(tinygltf::Scene const& scene);
  void append_to_asset(Asset& asset, tinygltf::Scene const& scene);

  struct Rendering_State
  {
    Technique_Ref cur_technique_i = -1;
    Material_Ref cur_material_i = -1;
  };

  void render_asset(Asset const& asset, gfx::Camera const& camera,
                    Rendering_State& cur_rendering_state);
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include "../common/maybe_owned.hpp"
#include "camera.h"

#include "idriver.h"
#include "ishader.h"
#include "imesh.h"
#include "itexture.h"
#include "ibuffer.h"
#include "deferred.h"

#include <vector>
#include <unordered_map>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include "../../gltf/tiny_gltf_loader.h"

#include "common.h"

namespace redc { namespace gfx
{
  struct Buffer
  {
    // Target type
    Buffer_Target target;
    // Cached (if GPU target) or actual data (if CPU buf target)
    std::vector<uint8_t> data;
    // Optional GPU buffer representation
    std::unique_ptr<IBuffer> repr;
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

  struct Program
  {
    // A program owns its representation and they are not shared with other
    // programs.
    std::unique_ptr<IShader> repr;

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

    // Value and type of the parameter. I chose not to put a Typed_Value here
    // and instead use both a type and value explicitely because the value here
    // is optional I think, whereas type is not. This is distinct from a value
    // provided in material.values.
    Value_Type type;
    boost::optional<Value> default_value;

    boost::optional<Param_Semantic> semantic;
    Param_Bind bind;
  };
  struct Attrib_Decl
  {
    Value_Type type;
    boost::optional<Attrib_Semantic> semantic;
    Attrib_Bind bind;
  };

  struct Technique
  {
    ~Technique() {}

    Program_Ref program_i;

    // This includes name, type and bind information.
    std::unordered_map<std::string, Param_Decl> parameters;
    std::unordered_map<std::string, Attrib_Decl> attributes;

    bool is_deferred;
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
    std::vector<std::pair<Param_Bind, Typed_Value> > parameters;
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
    Primitive_Type mode;
  };

  struct Mesh
  {
    // A mesh will own its representation so that in the best case (only one set
    // of primitives) we don't have to constantly reformat the vertex array
    // object.
    std::unique_ptr<IMesh> repr;

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
    std::vector<std::unique_ptr<ITexture> > textures;

    std::vector<Accessor> accessors;

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

  Asset load_asset(IDriver& driver, tinygltf::Scene const& scene);
  void append_to_asset(IDriver& driver, Asset& ret, tinygltf::Scene const& scene);

  Attrib_Bind get_attrib_semantic_bind(Technique const& tech,
                                       Attrib_Semantic attrib_semantic);

  Param_Bind get_param_semantic_bind(Technique const& tech,
                                     Param_Semantic param_semantic);
} }

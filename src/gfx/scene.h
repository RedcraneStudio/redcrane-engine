/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#if defined(REDC_USE_OPENGL)
#include "glad/glad.h"
#endif

#include "../common/maybe_owned.hpp"

#include <vector>
#include <unordered_map>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include "../../gltf/tiny_gltf_loader.h"
namespace redc
{
#if defined(REDC_USE_OPENGL)

  // In OpenGL, attributes are vertex shader input variables.
  struct Attribute_Bind
  {
    GLint loc;
  };

  // Parameters are just uniforms.
  struct Parameter_Bind
  {
    GLint loc;
  };

  // A VBO
  struct Buf
  {
    GLuint buf;
  };

  // A texture object
  struct Texture_Repr
  {
    GLuint tex;
  };

  // A VAO
  struct Mesh_Repr
  {
    GLuint vao;
  };

  // Shader
  struct Shader_Repr
  {
    GLuint shader;
  };

  // Program
  struct Program_Repr
  {
    GLuint program;
  };

#elif defined(REDC_USE_DIRECTX)
  struct Attribute_Bind {};
  struct Parameter_Bind {};
  struct Buf {};
  struct Texture_Repr {};
  struct Mesh_Repr {};
  struct Shader_Repr {};
  struct Program_Repr {};
#endif

#if defined(REDC_USE_OPENGL)
  enum class Data_Type : GLenum
  {
    Byte = GL_BYTE, UByte = GL_UNSIGNED_BYTE, Short = GL_SHORT,
    UShort = GL_UNSIGNED_SHORT, Int = GL_INT, UInt = GL_UNSIGNED_INT,
    Float = GL_FLOAT, Double = GL_DOUBLE
  };

  enum class Buffer_Target : GLenum
  {
    Array = GL_ARRAY_BUFFER, Element_Array = GL_ELEMENT_ARRAY_BUFFER
  };

  enum class Texture_Format : GLenum
  {
    Rgba = GL_RGBA
  };
  enum class Texture_Target : GLenum
  {
    Tex_2D = GL_TEXTURE_2D
  };

  enum class Render_Mode : GLenum
  {
    Points = GL_POINTS, Lines = GL_LINE, Line_Loop = GL_LINE_LOOP,
    Line_Strip = GL_LINE_STRIP, Triangles = GL_TRIANGLES,
    Triangle_Strip = GL_TRIANGLE_STRIP, Triangle_Fan = GL_TRIANGLE_FAN
  };

  enum class Shader_Type : GLenum
  {
    Vertex = GL_VERTEX_SHADER, Fragment = GL_FRAGMENT_SHADER
  };

#else
  enum class Data_Type
  {
    Byte, UByte, Short, UShort, Int, UInt, Float, Double
  };

  enum class Buffer_Target
  {
    Array, Element_Array
  }

  enum class Texture_Format
  {
    Rgb
  };
  enum class Texture_Target
  {
    Tex_2D
  };

  enum class Render_Mode
  {
    Points, Lines, Line_Loop, Line_Strip, Triangles, Triangle_Strip,
    Triangle_Fan
  };

  enum class Shader_Type
  {
    Vertex, Fragment
  };

#endif

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

  // Type of uniform (in opengl)
  enum class Param_Type
  {
    Sampler, Float, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4
  };

  using Texture_Ref = std::size_t;
  // Represents a uniform value
  struct Parameter
  {
    Param_Type type;
    // Matrices stored in column-major order.
    boost::variant<Texture_Ref, std::vector<float> > value;
  };

  // Represents a semantic uniform
  enum class Param_Semantic
  {
    Local, Model, View, Projection, Model_View, Model_View_Projection,
    Model_Inverse, View_Inverse, Projection_Inverse, Model_View_Inverse,
    Model_View_Projection_Inverse, Model_Inverse_Transpose,
    Model_View_Inverse_Transpose, Viewport, Joint_Matrix
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
    // by technique then by material.
    std::vector<std::pair<Parameter, Parameter_Bind> > params;
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
    std::unordered_map<std::string, Attribute_Bind> attributes;
  };

  using Program_Ref = std::size_t;
  enum class Technique_Parameter_Type
  {
    Byte, UByte, Short, UShort, Int, UInt, Float, Vec2, Vec3, Vec4, IVec2,
    IVec3, IVec4, Bool, BVec2, BVec3, BVec4, Mat2, Mat3, Mat4, Sampler2D
  };

  using Node_Ref = std::size_t;
  struct Technique_Parameter
  {
    // Optional node to take transformation from
    boost::optional<Node_Ref> node;

    // Should be 1 unless we are dealing with an array. I don't think this can
    // be more than one if we are dealing with an attribute.
    int count;

    // Type of the parameter
    Technique_Parameter_Type type;
    Parameter value;

    // Optional semantic meaning
    boost::optional<boost::variant<Param_Semantic, Attrib_Semantic> > semantic;
    // Either a parameter (uniform) or an attribute (input variable).
    boost::variant<Parameter_Bind, Attribute_Bind> bind;
  };

  struct Technique
  {
    ~Technique() {}

    Program_Ref program_i;

    // This includes name, type and bind information.
    std::unordered_map<std::string, Technique_Parameter> parameters;
#if 0
    // First use a mesh
    virtual void use_mesh(Mesh_Repr repr) = 0;

    // Use an accessor to bind the buffer properly to the attribute
    virtual void use_array_accessor(Attribute_Bind, Buf buf,
                                    Accessor const& acc) = 0;
    virtual void use_element_array_accessor(Buf buf, Accessor const& acc) = 0;
    virtual void disable_element_array() = 0;

    // Set technique parameters
    virtual void set_parameter(Parameter_Bind, Parameter const& param) = 0;

    // Use these to find the bind points in the first place
    virtual Parameter_Bind get_semantic_param_bind(Param_Semantic ps) = 0;
    virtual Attribute_Bind get_semantic_attrib_bind(Attrib_Semantic as) = 0;

    virtual Parameter_Bind get_param_bind(std::string ps) = 0;
    virtual Attribute_Bind get_attrib_bind(std::string as) = 0;

    // Render the current mesh from use_mesh
    // We need the asset because internally other nodes may be referenced.
    virtual void render(Asset const& asset) = 0;
#endif
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
    Node_Ref parent;

    boost::optional<std::array<float, 4> > rotation;
    boost::optional<std::array<float, 3> > scale;
    boost::optional<std::array<float, 3> > translation;
    boost::optional<std::array<float, 16> > matrix;
  };

  struct Asset
  {
    ~Asset();

    std::vector<Buf> buffers;
    std::vector<Texture_Repr> textures;

    std::vector<Accessor> accessors;

    std::vector<Shader> shaders;
    std::vector<Program> programs;
    std::vector<Technique> techniques;

    std::vector<Material> materials;

    std::vector<Mesh> meshes;
    std::vector<Node> nodes;
  };

  Asset load_asset(tinygltf::Scene const& scene);
}

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
    GLuint loc;
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

#elif defined(REDC_USE_DIRECTX)
  struct Attribute_Bind {};
  struct Parameter_Bind {};
  struct Buf {};
  struct Texture_Repr {};
  struct Mesh_Repr {};
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
    boost::variant<Texture_Repr, std::vector<float> > value;
  };

  // Represents a semantic uniform
  struct Param_Semantic
  {
    enum
    {
      Local, Model, View, Projection, Model_View, Model_View_Projection,
      Model_Inverse, View_Inverse, Projection_Inverse, Model_View_Inverse,
      Model_View_Projection_Inverse, Model_Inverse_Transpose,
      Model_View_Inverse_Transpose, Viewport, Joint_Matrix
    } kind;
  };

  // Techniques do most of the work.
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

    unsigned short index;
  };

  // Because the material of a primitive is not going to change, we can
  // statically bind accessors to shader bind locations using information we
  // know about the material.

  using Accessor_Ref = std::size_t;
  using Material_Ref = std::size_t;
  struct Primitive
  {
    // Accessors bound to attributes. There is no different in the use of
    // semantic and non-semantic accessors, because they are bound to a bind
    // point early.
    std::vector<std::pair<Accessor_Ref, Attribute_Bind> > accessors;
    boost::optional<Accessor_Ref> indices;
    Material_Ref mat_i;
    Render_Mode mode;
  };

  struct Technique
  {
    ~Technique() {}

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
    virtual void render() = 0;
  };

  std::unique_ptr<Technique> make_diffuse_technique();

  using Mesh_Repr_Ref = std::size_t;
  struct Mesh
  {
    Mesh_Repr_Ref repr_i;
    std::vector<Primitive> primitives;
  };
  struct Node
  {
    std::vector<Node> children;

    boost::optional<std::array<double, 4> > rotation;
    boost::optional<std::array<double, 3> > scale;
    boost::optional<std::array<double, 3> > translation;
    boost::optional<std::array<double, 16> > matrix;

    std::vector<Mesh> meshes;
  };
  struct Scene
  {
    std::vector<Buf> buffers;
    std::vector<Accessor> accessors;

    std::vector<Texture_Repr> textures;
    std::vector<std::unique_ptr<Technique> > techniques;
    std::vector<Material> materials;

    std::vector<Mesh_Repr> mesh_reprs;
    std::vector<Mesh> meshes;

    Node root;
  };

  Scene load_scene(tinygltf::Scene const& scene);
}

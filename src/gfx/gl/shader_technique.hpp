/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <algorithm>
#include "glad/glad.h"
#include <string>
#include "../../common/debugging.h"
#include "../scene.h"
namespace redc
{
  #if 0
  struct GL_Shader_Technique : public Technique
  {
    GLuint cur_vao;
    GLuint program;

    std::size_t min_array_count;

    GLuint cur_element_array;
    Accessor element_array_accessor;

    // Tags to locations
    using tag_t = std::string;
    std::unordered_map<tag_t, GLint> tags;

    // Strings to vertex array attributes
    std::unordered_map<std::string, GLuint> attribs;

    void set_var_tag(std::string str, tag_t tag);

    void use_mesh(Mesh_Repr repr) override
    {
      if(cur_vao != repr.vao)
      {
        glBindVertexArray(repr.vao);
        cur_vao = repr.vao;
      }
    }
    void use_array_accessor(Attribute_Bind bind, Buf buf,
                            Accessor const& acc) override
    {
      // We have some data
      glBindBuffer(GL_ARRAY_BUFFER, buf.buf);

      // We can't be dealing with matrices of any kind
      REDC_ASSERT((int) acc.attrib_type & 0x10);

      // Bind the buffer to a given attribute, at this point we know exactly
      // what part of the buffer needs to be referenced.
      glVertexAttribPointer(bind.loc, (GLint) acc.attrib_type & 0x0f,
                            (GLenum) acc.data_type, GL_FALSE,
                            acc.stride, (void*) acc.offset);

      // Contribute to the minimum of elements that are available, the minimum
      // will be used to glDrawArrays.
      min_array_count = std::min(min_array_count, acc.count);
    }
    void use_element_array_accessor(Buf buf, Accessor const& acc) override
    {
      if(cur_element_array != buf.buf)
      {
        // Use this as our element array
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.buf);

        cur_element_array = buf.buf;
      }
      // Keep this for later when we need to render.
      element_array_accessor = acc;
    }
    void disable_element_array() override
    {
      cur_element_array = 0;
    }

    void set_parameter(Parameter_Bind bind, Parameter const& param) override;

    Parameter_Bind get_semantic_param_bind(Param_Semantic ps) override;
    Attribute_Bind get_semantic_attrib_bind(Attrib_Semantic as) override;

    Parameter_Bind get_param_bind(std::string ps) override
    {
      auto loc = tags.find(ps);
      REDC_ASSERT(loc != tags.end());
      return Parameter_Bind{loc->second};
    }
    Attribute_Bind get_attrib_bind(std::string as) override
    {
      auto loc = attribs.find(as);
      REDC_ASSERT(loc != attribs.end());
      return Attribute_Bind{loc->second};
    }

    void render() override;

  };

#define LOC_BAIL(location) \
  //REDC_ASSERT_MSG(location != -1, "Bad uniform location in shader");  \
  if(location == -1) return

  void GL_Shader_Technique::set_var_tag(std::string var, tag_t tag)
  {
    // Get the location, we don't need to switch to our program to do this!
    auto loc = glGetUniformLocation(program, var.data());

    // If it's not valid don't put it in!
    LOC_BAIL(loc);

    // Commit it, this is basically insert_or_assign but we don't have C++17 yet
    // so this will have to do.
    if(tags.count(tag)) tags.at(tag) = loc;
    else tags.insert({tag, loc});
  }
#undef LOC_BAIL

  void GL_Shader_Technique::set_parameter(Parameter_Bind bind,
                                          Parameter const& param)
  {
#ifdef REDC_OPENGL_4
    if(param.type == Param_Type::Sampler)
    {
      glProgramUniform1i(program, bind.loc,
                         boost::get<Texture_Repr>(param.value).tex);
    }
    else
    {
      auto ptr = &boost::get<0>(param.value)[0];
      switch(param.type)
      {
      case Param_Type::Float:
        glProgramUniform1fv(program, bind.loc, ptr);
        break;
      case Param_Type::Vec2:
        glProgramUniform2fv(program, bind.loc, ptr);
        break;
      case Param_Type::Vec3:
        glProgramUniform3fv(program, bind.loc, ptr);
        break;
      case Param_Type::Vec4:
        glProgramUniform4fv(program, bind.loc, ptr);
        break;
      case Param_Type::Mat2:
        glProgramUniformMatrix2fv(program, bind.loc, ptr);
        break;
      case Param_Type::Mat3:
        glProgramUniformMatrix3fv(program, bind.loc, ptr);
        break;
      case Param_Type::Mat4:
        glProgramUniformMatrix4fv(program, bind.loc, ptr);
        break;
      }
    }
#else
    // We have no way of knowing whether the current program has been changed
    // since the last invocation of this function so we have to rebind the
    // program.
    // TODO: Add some shared state parameter techniques can use to store these
    // things.

    glUseProgram(program);
    if(param.type == Param_Type::Sampler)
    {
      glUniform1i(bind.loc, boost::get<Texture_Repr>(param.value).tex);
    }
    else
    {
      auto ptr = &boost::get<std::vector<float> >(param.value)[0];
      switch(param.type)
      {
      case Param_Type::Float:
        glUniform1fv(bind.loc, 1, ptr);
        break;
      case Param_Type::Vec2:
        glUniform2fv(bind.loc, 2, ptr);
        break;
      case Param_Type::Vec3:
        glUniform3fv(bind.loc, 3, ptr);
        break;
      case Param_Type::Vec4:
        glUniform4fv(bind.loc, 4, ptr);
        break;
      case Param_Type::Mat2:
        glUniformMatrix2fv(bind.loc, 1, GL_FALSE, ptr);
        break;
      case Param_Type::Mat3:
        glUniformMatrix3fv(bind.loc, 1, GL_FALSE, ptr);
        break;
      case Param_Type::Mat4:
        glUniformMatrix4fv(bind.loc, 1, GL_FALSE, ptr);
        break;
      default:
        REDC_ASSERT_MSG(false, "Bad param type");
        break;
      }
    }
#endif
  }

  Parameter_Bind GL_Shader_Technique::get_semantic_param_bind(Param_Semantic ps)
  {
    // Build a string
    tag_t tag;

    switch(ps.kind)
    {
    case Param_Semantic::Local:
      tag = "LOCAL";
      break;
    case Param_Semantic::Model:
      tag = "MODEL";
      break;
    case Param_Semantic::View:
      tag = "VIEW";
      break;
    case Param_Semantic::Projection:
      tag = "PROJECTION";
      break;
    case Param_Semantic::Model_View:
      tag = "MODELVIEW";
      break;
    case Param_Semantic::Model_View_Projection:
      tag = "MODELVIEWPROJECTION";
      break;
    case Param_Semantic::Model_Inverse:
      tag = "MODELINVERSE";
      break;
    case Param_Semantic::View_Inverse:
      tag = "VIEWINVERSE";
      break;
    case Param_Semantic::Projection_Inverse:
      tag = "PROJECTIONINVERSE";
      break;
    case Param_Semantic::Model_View_Inverse:
      tag = "MODELVIEWINVERSE";
      break;
    case Param_Semantic::Model_View_Projection_Inverse:
      tag = "MODELVIEWPROJECTIONINVERSE";
      break;
    case Param_Semantic::Model_Inverse_Transpose:
      tag = "MODELINVERSETRANSPOSE";
      break;
    case Param_Semantic::Model_View_Inverse_Transpose:
      tag = "MODELVIEWINVERSETRANSPOSE";
      break;
    case Param_Semantic::Viewport:
      tag = "VIEWPORT";
      break;
    case Param_Semantic::Joint_Matrix:
      tag = "JOINTMATRIX";
      break;
    default:
      REDC_ASSERT_MSG(!tag.empty(), "Invalid parameter semantic");
      break;
    };

    // Return the location in the form of a Parameter_Bind
    auto loc = tags.find(tag);
    REDC_ASSERT(loc != tags.end());

    return Parameter_Bind{loc->second};
  }
  Attribute_Bind GL_Shader_Technique::get_semantic_attrib_bind(Attrib_Semantic as)
  {
    std::string ref;
    switch(as.kind)
    {
    case Attrib_Semantic::Position:
      ref = "POSITION";
      break;
    case Attrib_Semantic::Normal:
      ref = "NORMAL";
      break;
    case Attrib_Semantic::Texcoord:
      ref = "TEXCOORD";
      break;
    case Attrib_Semantic::Color:
      ref = "COLOR";
      break;
    case Attrib_Semantic::Joint:
      ref = "JOINT";
      break;
    case Attrib_Semantic::Weight:
      ref = "WEIGHT";
      break;
    default:
      REDC_ASSERT_MSG(!ref.empty(), "Invalid attribute semantic");
      break;
    }

    ref += "_" + std::to_string(as.index);

    // Find the attribute index.
    auto loc = attribs.find(ref);
    REDC_ASSERT(loc != attribs.end());
    return Attribute_Bind{loc->second};
  }

  void GL_Shader_Technique::render()
  {
    // We assume that the vao and element array is bound and the user has
    // already set the format of each attribute / buffers, etc.
    if(cur_element_array)
    {
      // Stride isn't supported
      REDC_ASSERT(element_array_accessor.stride == 0);

      // It's already bound, use the accessor to draw the elements
      glDrawElements(GL_TRIANGLES, element_array_accessor.count,
                     (GLenum) element_array_accessor.data_type,
                     (GLvoid*) element_array_accessor.offset);
    }
    else
    {
      glDrawArrays(GL_TRIANGLES, 0, min_array_count);
    }
  }

  std::unique_ptr<Technique> make_diffuse_technique()
  {
    return std::make_unique<GL_Shader_Technique>();
  }
#endif
}

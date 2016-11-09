/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include "../ishader.h"
#include "glad/glad.h"
namespace redc { namespace gfx { namespace gl
{
  struct Driver;
  struct GL_Shader : public IShader
  {
    GL_Shader(Driver& driver);
    ~GL_Shader();

    void reinitialize() override;

    void load_vertex_part(shader_source_t const&,
                          std::string const&) override;
    void load_fragment_part(shader_source_t const&,
                            std::string const&) override;
    void load_geometry_part(shader_source_t const&,
                            std::string const&) override;

    bool link() override;
    bool linked() override { return linked_; }

    void set_var_tag(tag_t tag, std::string var_name) override;

    void set_vec2(Param_Bind, float const*) override;
    void set_vec2(Param_Bind bind, glm::vec2 const& vec) override;

    void set_vec3(Param_Bind, float const*) override;
    void set_vec3(Param_Bind bind, glm::vec3 const& vec) override;

    void set_vec4(Param_Bind, float const*) override;
    void set_vec4(Param_Bind bind, glm::vec4 const& vec) override;

    void set_ivec2(Param_Bind, int const*) override;
    void set_ivec2(Param_Bind bind, glm::ivec2 const& vec) override;

    void set_ivec3(Param_Bind, int const*) override;
    void set_ivec3(Param_Bind bind, glm::ivec3 const& vec) override;

    void set_ivec4(Param_Bind, int const*) override;
    void set_ivec4(Param_Bind bind, glm::ivec4 const& vec) override;

    void set_bvec2(Param_Bind, bool const*) override;
    void set_bvec2(Param_Bind bind, glm::bvec2 const& vec) override;

    void set_bvec3(Param_Bind, bool const*) override;
    void set_bvec3(Param_Bind bind, glm::bvec3 const& vec) override;

    void set_bvec4(Param_Bind, bool const*) override;
    void set_bvec4(Param_Bind bind, glm::bvec4 const& vec) override;

    void set_mat2(Param_Bind, float const*) override;
    void set_mat3(Param_Bind, float const*) override;
    void set_mat4(Param_Bind, float const*) override;

    void set_float(Param_Bind, float) override;
    void set_integer(Param_Bind, int) override;
    void set_bool(Param_Bind, bool) override;

    // Active our shader program, this is a dangerous operation because it
    // changes OpenGL state!
    void use();
    // Get the actual location of a given tag / uniform
    GLint get_location_from_tag(tag_t) const;

    Attrib_Bind get_attrib_bind(std::string attrib) const;
    Param_Bind get_param_bind(std::string param) const;
    Param_Bind get_tag_param_bind(std::string tag) const;

  private:
    Driver* driver_;

    // Shader objects
    GLuint v_shade_ = 0;
    GLuint f_shade_ = 0;
    GLuint g_shade_ = 0;

    // Program object
    GLuint prog_;

    // Whether we are linked or not
    bool linked_ = false;

    void allocate_shader_();
    void unallocate_shader_();

    // Hash map of tags of strings => uniform locations
    std::unordered_map<tag_t, GLint> tags;
    std::unordered_map<tag_t, std::string> tag_vars_;

    void load_part(shader_source_t const& source, std::string name,
                   GLenum part, GLuint& shade_obj);
  };
} } }

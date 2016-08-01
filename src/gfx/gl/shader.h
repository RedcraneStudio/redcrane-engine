/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <unordered_map>
#include "../shader.h"
#include "glad/glad.h"
namespace redc { namespace gfx { namespace gl
{
  struct Driver;
  struct GL_Shader : public Shader
  {
    GL_Shader(Driver& driver);
    ~GL_Shader();

    void load_vertex_part(shader_source_t const&,
                          std::string const&) override;
    void load_fragment_part(shader_source_t const&,
                            std::string const&) override;
    void load_geometry_part(shader_source_t const&,
                            std::string const&) override;

    bool link() override;
    bool linked() override { return linked_; }

    void set_var_tag(tag_t tag, std::string var_name) override;

    void set_mat4(tag_t, glm::mat4 const&) override;

    void set_integer(tag_t, int) override;

    void set_vec2(tag_t, glm::vec2 const&) override;
    void set_vec3(tag_t, glm::vec3 const&) override;
    void set_vec4(tag_t, glm::vec4 const&) override;

    void set_float(tag_t, float) override;

    // Active our shader program, this is a dangerous operation because it
    // changes OpenGL state!
    void use();
    // Get the actual location of a given tag / uniform
    GLint get_location_from_tag(tag_t);

    Param_Bind get_tag_bind(tag_t tag) override;

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

    // Hash map of tags of strings => uniform locations
    std::unordered_map<tag_t, GLint> tags;

    void load_part(shader_source_t const& source, std::string name,
                   GLenum part, GLuint& shade_obj);
  };
} } }

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../shader.h"
#include "glad/glad.h"
namespace game { namespace gfx { namespace gl
{
  struct GL_Shader : public Shader
  {
    GL_Shader() noexcept;
    ~GL_Shader() noexcept;

    void load_vertex_part(std::string const&) noexcept override;
    void load_fragment_part(std::string const&) noexcept override;

    int get_location(std::string const&) noexcept override;

    void set_matrix(int, glm::mat4 const&) noexcept override;

    void set_sampler(int, unsigned int) noexcept override;

    void set_vec2(int, glm::vec2 const&) noexcept override;
    void set_vec3(int, glm::vec3 const&) noexcept override;
    void set_vec4(int, glm::vec4 const&) noexcept override;

    void set_float(int, float) noexcept override;

    void use() noexcept;
  private:
    GLuint v_shade_;
    GLuint f_shade_;
    GLuint prog_;
    bool linked_ = false;

    void try_load_() noexcept;
  };
} } }

/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#version 330
in vec3 position;
in vec3 normal;
in vec2 uv_in;

uniform mat4 model_view;
uniform mat3 normal_matrix;
uniform mat4 proj_matrix;

out vec3 normal_world;
out vec2 uv;

void main()
{
  normal_world = normal_matrix * normal;

  uv = uv_in;

  vec3 world_pos = vec3(model_view * vec4(position, 1.0f));
  gl_Position = proj_matrix * vec4(world_pos, 1.0f);
}

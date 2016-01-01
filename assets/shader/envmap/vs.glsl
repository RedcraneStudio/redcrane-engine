/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#version 330

layout(location = 0) in vec3 pos;

uniform mat4 view;
uniform mat4 proj;

out vec3 tex_coords;

void main()
{
  tex_coords = pos;
  gl_Position = proj * view * vec4(pos, 1.0);
}

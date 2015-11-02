#version 330
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 uv_in;

out vec2 uv;

// Multiplied by heightmap value to get value to add.
uniform float max_height_adjust;

uniform sampler2D heightmap;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
  vec3 adjusted_vert = vertex +
                       vec3(0.0, texture(heightmap, uv_in).r * max_height_adjust,
                            0.0);
  gl_Position = proj * view * model * vec4(adjusted_vert, 1.0);

  uv = uv_in;
}

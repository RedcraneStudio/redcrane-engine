#version 330
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 uv_in;

out vec2 uv;
out vec3 normal_world;
out vec3 light_dir;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
  // Calculate our mvp matrix
  mat4 mvp = proj * view * model;

  // Calculate the vertex position and tell OpenGL about it.
  vec4 pos = mvp * vec4(vertex, 1.0);
  gl_Position = pos;

  // Calculate the vertex normal.
  normal_world = vec3(model * vec4(normal_in, 0.0));

  // Fill in this information for the light direction.
  uv = uv_in;
}

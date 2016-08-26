#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 uv_in;

out vec2 uv;
out vec4 world_pos;
out vec3 world_normal;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
  // Calculate our world position and screen space position.
  world_pos = model * vec4(vertex, 1.0);
  gl_Position = proj * view * world_pos;

  uv = uv_in;

  // Calculate the vertex normal.
  world_normal = vec3(model * vec4(normal_in, 0.0));
}

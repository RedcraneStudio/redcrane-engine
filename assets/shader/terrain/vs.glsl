#version 330
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 uv_in;

out vec2 uv;
out vec3 world_pos;

// Multiplied by heightmap value to get value to add.
uniform float max_height_adjust;

uniform sampler2D heightmap;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
  float height = texture(heightmap, uv_in).r * max_height_adjust;
  vec3 adj_vert = vertex + vec3(0.0, height, 0.0);

  world_pos = vec3(model * vec4(adj_vert, 1.0));
  gl_Position = proj * view * vec4(world_pos, 1.0);

  uv = uv_in;
}

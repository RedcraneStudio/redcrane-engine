#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat3 normal_mat;

uniform mat4 modelview;
uniform mat4 proj;

out vec3 position_world;
out vec3 normal_world;

void main()
{
  normal_world = normal_mat * normal;
  position_world = vec3(modelview * vec4(position, 1.0));

  // Screen space position
  gl_Position = proj * vec4(position_world, 1.0);
}

#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat3 normal_mat;

uniform mat4 modelview;
uniform mat4 proj;

out vec3 position_cam;
out vec3 normal_cam;

void main()
{
  normal_cam = normal_mat * normal;
  position_cam = vec3(modelview * vec4(position, 1.0));

  // Screen space position
  gl_Position = proj * vec4(position_cam, 1.0);
}

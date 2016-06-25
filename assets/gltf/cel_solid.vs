#version 330
in vec3 position;
in vec3 normal;

uniform mat4 model_view;
uniform mat3 normal_matrix;
uniform mat4 proj_matrix;

out vec3 normal_world;

void main()
{
  normal_world = normal_matrix * normal;

  vec3 world_pos = vec3(model_view * vec4(position, 1.0f));
  gl_Position = proj_matrix * vec4(world_pos, 1.0f);
}

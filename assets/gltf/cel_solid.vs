#version 330
in vec3 position;
in vec3 normal;

uniform mat4 model_view;
uniform mat3 normal_matrix;
uniform mat4 proj_matrix;

void main()
{
  gl_Position = proj_matrix * model_view * vec4(position, 1.0f);
}

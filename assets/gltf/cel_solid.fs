#version 330

in vec3 normal_world;

out vec4 color;

uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 emission;
uniform vec4 specular;
uniform float shininess;

void main()
{
  color = diffuse;
  //color = vec4(normal_world, 1.0f);
}

#version 330

out vec4 color;

uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 emission;
uniform vec4 specular;
uniform float shininess;

void main()
{
  color = diffuse;
}

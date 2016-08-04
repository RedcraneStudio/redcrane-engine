#version 330
layout(location = 0) out vec4 pos;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 color;

in vec3 position_world;
in vec3 normal_world;

uniform vec3 diffuse;
uniform float shininess;

void main()
{
  pos = vec4(position_world, 1.0);
  normal = vec4(normal_world, 1.0);
  color = vec4(diffuse, shininess);
}

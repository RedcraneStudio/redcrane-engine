#version 440
layout(location = 0) in vec3 vertex;
layout(location = 1) out vec3 pos;

uniform mat4 mvp;

void main()
{
  gl_Position = mvp * vec4(vertex, 1.0);
  pos = vertex;
}

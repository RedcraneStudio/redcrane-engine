#version 330
layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 uv_in;

out vec2 uv;

uniform mat4 ortho;

void main()
{
  gl_Position = ortho * vec4(vertex, 1.0, 1.0);
  uv = uv_in;
}

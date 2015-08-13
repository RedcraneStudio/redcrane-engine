#version 330
layout(location = 0) in vec2 vertex;

layout(location = 1) in vec2 uv_in;
layout(location = 2) in vec4 color_in;

out vec2 uv;
out vec4 color;

uniform mat4 ortho;

void main()
{
  gl_Position = ortho * vec4(vertex, 0.0, 1.0);
  uv = uv_in;
  color = color_in;
}

#version 330

layout(location = 0) out vec4 diffuse_out;

in vec2 uv;
in vec4 color;

uniform sampler2D tex;

void main()
{
  diffuse_out = texture(tex, uv);
  diffuse_out *= color;
}

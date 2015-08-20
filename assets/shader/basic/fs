#version 330

layout(location = 0) out vec4 diffuse_out;

in vec2 uv;
in vec3 normal_world;

uniform vec3 light_dir;
uniform vec4 dif;
uniform sampler2D tex;

void main()
{
  diffuse_out = texture(tex, uv);
  diffuse_out.r *= dif.r;
  diffuse_out.g *= dif.g;
  diffuse_out.b *= dif.b;
  diffuse_out.a = 1.0;

  float light_intensity = dot(normalize(normal_world), light_dir);
  float ambient_intensity = .85f;
  diffuse_out *= max(light_intensity, ambient_intensity);
}

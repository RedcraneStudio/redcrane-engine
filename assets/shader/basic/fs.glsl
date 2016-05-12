#version 330

layout(location = 0) out vec4 diffuse_out;

in vec2 uv;
in vec4 world_pos;
in vec3 world_normal;

uniform vec3 light_pos;
uniform vec4 dif;
uniform sampler2D tex;

void main()
{
  // diffuse_out = texture(tex, uv) * dif;
  diffuse_out = dif;
  // diffuse_out.a = 1.0;

  float light_intensity = dot(normalize(world_normal),
                              normalize(light_pos - vec3(world_pos)));
  float ambient_intensity = .01;
  diffuse_out.rgb *= max(light_intensity, ambient_intensity);
}

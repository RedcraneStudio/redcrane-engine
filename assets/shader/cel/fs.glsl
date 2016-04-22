#version 330

layout(location = 0) out vec4 diffuse_out;

in vec2 uv;
in vec4 world_pos;
in vec3 world_normal;

uniform vec3 light_pos;
uniform vec4 dif;

void main()
{
  diffuse_out = dif;

  float light_intensity = dot(normalize(world_normal),
                              normalize(light_pos - vec3(world_pos)));

  // Quantize light intensity
  int n_shades = 3;
  light_intensity = ceil(light_intensity * n_shades) / n_shades;

  float ambient_intensity = .25;
  diffuse_out.rgb *= max(light_intensity, ambient_intensity);
}

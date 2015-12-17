#version 330

layout(location = 0) out vec4 dif;

// Uv coordinates of fragment
in vec2 uv;

// World position of fragment
in vec3 world_pos;

uniform float ambient_intensity;

// Light position in world space
uniform vec3 light_pos;
uniform vec3 camera_pos;

uniform sampler2D normalmap;
uniform sampler2D diffusemap;

// Model matrix to transform normals into world space.
uniform mat4 model;

float fresnel(float n1, float n2, float cos_theta)
{
  float r0 = (n1 - n2) / (n1 + n2);
  r0 = r0 * r0;

  return r0 + (1 - r0) * pow(1 - cos_theta, 5);
}

void main()
{
  dif = vec4(.3, .5, .9, 1.0);

  vec3 normal_world = normalize(vec3(model * texture(normalmap, uv)));
  vec3 light_dir = normalize(light_pos - world_pos);
  vec3 view_dir = normalize(camera_pos - world_pos);

  vec3 halfway = normalize(light_dir + view_dir);

  float fr = fresnel(1.0, 1.33, dot(halfway, light_dir));
  fr = max(0.0, min(fr, 1.0));

  float diffuse_intensity = max(0.0, dot(normal_world, light_dir));
  float specular_intensity = pow(max(0.0, dot(normal_world, halfway)), 4);
  dif.rgb *= mix(diffuse_intensity, specular_intensity, fr) + ambient_intensity;
}

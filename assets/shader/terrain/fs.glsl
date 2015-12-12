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

float fresnel(float n1, float n2, vec3 light_dir, vec3 normal)
{
  float r0 = (n1 - n2) / (n1 + n2);
  r0 = r0 * r0;

  return r0 + (1 - r0) * pow(1 - dot(light_dir, normal), 5);
}

void main()
{
  //dif = texture(diffusemap, uv);
  dif = vec4(.3, .5, .9, 1.0);

  vec3 normal_world = normalize(vec3(model * texture(normalmap, uv)));
  vec3 light_dir = normalize(light_pos - world_pos);
  vec3 view_dir = normalize(camera_pos - world_pos);

  vec3 halfway = normalize(light_dir + view_dir);

  float fr = fresnel(1.0, 1.33, light_dir, halfway);
  fr = max(0.0, min(fr, 1.0));

  float light_intensity = max(0.0, dot(normal_world, light_dir));
  float specular_intensity = pow(fr * max(0.0, dot(normal_world, halfway)), 4);
  dif.rgb *= light_intensity + specular_intensity + ambient_intensity;
}

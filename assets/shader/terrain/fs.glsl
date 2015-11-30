#version 330

layout(location = 0) out vec4 dif;

// Uv coordinates of fragment
in vec2 uv;

// World position of fragment
in vec3 world_pos;

uniform float ambient_intensity;

// Light position in world space
uniform vec3 light_pos;

uniform sampler2D normalmap;
uniform sampler2D diffusemap;

// Model matrix to transform normals into world space.
uniform mat4 model;

void main()
{
  dif = texture(diffusemap, uv);

  vec3 normal_world = vec3(model * texture(normalmap, uv));
  float light_intensity = dot(normalize(normal_world),
                              normalize(light_pos - world_pos));
  dif.rgb *= max(light_intensity, ambient_intensity);
}

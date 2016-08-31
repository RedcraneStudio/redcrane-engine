#version 330 core

layout(location = 0) out vec4 o_dst;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_color;

uniform vec4 u_viewport;

uniform vec3 u_light_pos;
uniform float u_light_power;
uniform vec3 u_light_diffuse_color;
uniform vec3 u_light_specular_color;

uniform float u_ambient;

uniform mat4 u_view;

uniform vec3 u_fog_color;
uniform float u_fog_start;
uniform float u_fog_end;

float fresnel(float n2, float theta)
{
  float r0 = pow(((1.0f - n2) / (1.0f + n2)), 2.0f);
  return r0 + (1.0f - r0) * pow(1.0f - theta, 5.0f);
}

void main()
{
  // Find screen coordinates
  vec2 uv = gl_FragCoord.xy / u_viewport.zw;

  // Find diffuse color and specularity
  vec4 diffuse = texture(u_color, uv);
  float shininess = diffuse.a;

  // Find normals and position
  vec4 position = texture(u_position, uv);
  vec4 normal_sample = texture(u_normal, uv);
  vec3 normal = vec3(normal_sample);
  float w_coord = normal_sample.w;

  // Idea, instead of rendering to the default framebuffer just render onto the
  // diffuse buffer and use blending as we use here, then do a single pass to
  // write *that* texture to GL_BACK_LEFT. At this point we can do gamma
  // correction.

  if(position.w <= 0.0f || 1.0f <= position.w) discard;

  // Convert the light position to camera space
  vec3 light_pos = vec3(u_view * vec4(u_light_pos, 1.0f));
  // Calculate the direction from the surface to the camera
  vec3 light_dir = normalize(light_pos - vec3(position));

  // We are operating in camera space, so this is always the camera direction
  vec3 view_dir = vec3(0.0f, 0.0f, 1.0f);

  float lambertian = max(dot(light_dir, normal), 0.0f);

  vec3 halfway = normalize(view_dir + light_dir);
  float spec_angle = max(dot(halfway, normal), 0.0f);
  float specular = pow(spec_angle, shininess);

  o_dst = vec4(diffuse.rgb * u_ambient, 1.0);

  specular = 0.0;

  vec3 light_contrib = vec3(u_light_diffuse_color * lambertian) +
                       vec3(u_light_specular_color * specular);
  o_dst.rgb += light_contrib * u_light_power;

  // Use the right depth so we get depth testing
  gl_FragDepth = position.w;

  // TODO: More post-processing
  float fog_coord = position.w / w_coord;
  float fog_factor = (u_fog_end - fog_coord) / (u_fog_end - u_fog_start);
  fog_factor = 1.0f - clamp(fog_factor, 0.0f, 1.0f);
  o_dst = mix(o_dst, vec4(u_fog_color, 1.0f), fog_factor);
}

#version 330

layout(location = 0) out vec4 o_dst;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_color;

uniform vec4 u_viewport;

uniform vec3 u_light_pos;
uniform float u_light_power;
uniform vec3 u_light_diffuse_color;
uniform vec3 u_light_specular_color;

uniform mat4 u_view;

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
  vec3 normal = vec3(texture(u_normal, uv));

  // Idea, instead of rendering to the default framebuffer just render onto the
  // diffuse buffer and use blending as we use here, then do a single pass to
  // write *that* texture to GL_BACK_LEFT. At this point we can do gamma
  // correction.

  if(position.w <= 0.0f) discard;

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

  o_dst = vec4(u_light_diffuse_color * lambertian, 1.0f) +
    vec4(u_light_specular_color * specular, 1.0f);

  o_dst.rgb *= u_light_power;

  // TODO: More post-processing
}

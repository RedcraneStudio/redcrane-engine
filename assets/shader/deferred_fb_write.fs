#version 330 core

layout(location = 0) out vec4 o_dst;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_color;

uniform vec4 u_viewport;

struct Light
{
  mat4 model;
  vec3 color;

  // This is the total power, a value of 1.0 is normal.
  float intensity;
  float dist;

  // These should add up to one
  float constant_attenuation;
  float linear_attenuation;
  float quadratic_attenuation;

  float fall_off_angle;
  float fall_off_exponent;
};
uniform Light u_cur_light;

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
  float fog_coord = normal_sample.w;

  // Use the right depth so we get depth testing
  gl_FragDepth = position.w;

  // Idea, instead of rendering to the default framebuffer just render onto the
  // diffuse buffer and use blending as we use here, then do a single pass to
  // write *that* texture to GL_BACK_LEFT. At this point we can do gamma
  // correction.

  if(position.w <= 0.0f || 1.0f <= position.w) discard;

  // Calculate ambient lighting.
  o_dst = vec4(diffuse.rgb * u_ambient, 1.0);

  // = Lighting

  // We are operating in camera space, so this is always towards the camera
  vec3 view_dir = vec3(0.0f, 0.0f, 1.0f);

  // Convert the light position to camera space
  vec3 light_pos = vec3(u_view * u_cur_light.model *
                        vec4(0.0f, 0.0f, 0.0f, 1.0f));

  // Calculate the direction from the surface to the light,
  vec3 surface_to_light_dir = light_pos - vec3(position);
  float surface_to_light_dist = length(surface_to_light_dir);

  // Don't forget to normalize because this is a direction.
  surface_to_light_dir = normalize(surface_to_light_dir);

  // Find the direction that the light is supposed to be pointing, we use this
  // for spot lights to actually make a spot light.

  // All lights initially point downward.
  vec3 light_forward = normalize(
    vec3(u_view * u_cur_light.model * vec4(0.0f, 0.0f, -1.0f, 0.0f))
  );

  // == Diffuse intensity / ratio
  float lambertian = max(dot(surface_to_light_dir, normal), 0.0f);

  // == Specular intensity
  vec3 halfway = normalize(view_dir + surface_to_light_dir);
  float spec_angle = max(dot(halfway, normal), 0.0f);
  float specular = pow(spec_angle, shininess);

  // == Attenuation values
  float const_at = u_cur_light.intensity * u_cur_light.constant_attenuation;

  float linear_at = 0.0f;
  float quad_at = 0.0f;

  // Scale the distance by the light's maximum distance
  float scaled_distance = surface_to_light_dist / u_cur_light.dist;

  // Avoid dividing by zero and extending past the light's max distance (we
  // might change the latter behavior)!
  if(scaled_distance > 0.0)
  {
    if(u_cur_light.linear_attenuation > 0.0f)
    {
      linear_at = u_cur_light.intensity;
      linear_at /= scaled_distance * u_cur_light.linear_attenuation;
    }

    if(u_cur_light.quadratic_attenuation > 0.0f)
    {
      quad_at = u_cur_light.intensity;
      quad_at /= pow(scaled_distance, 2.0) *
        u_cur_light.quadratic_attenuation;
    }
  }

  // If the angle between the light's forward vector and the light direction
  // (from surface to light point) is too large, cancel the lighting
  if(u_cur_light.fall_off_angle < acos(dot(light_forward,
                                           -surface_to_light_dir)))
  {
    // The light doesn't go this far!
    // TODO: Use fall_off_exponent for this transition
    lambertian = 0.0f;
    specular = 0.0f;
  }

  vec3 light_contrib = u_cur_light.color * (lambertian + specular);

  // Attenuate total light contribution as a whole.
  float total_attenuation = max(max(const_at, linear_at), quad_at);
  light_contrib *= total_attenuation;

  // Add this light contribution to (possibly) ambient-lit fragment.
  o_dst.rgb += light_contrib;
  //o_dst.rgb = normalize(light_world.rgb);
  //o_dst.a = 1.0;
  //o_dst.rgb += light_contrib;

  // TODO: More post-processing
  float fog_factor = (u_fog_end - fog_coord) / (u_fog_end - u_fog_start);
  fog_factor = 1.0f - clamp(fog_factor, 0.0f, 1.0f);
  o_dst = mix(o_dst, vec4(u_fog_color, 1.0f), fog_factor);
}

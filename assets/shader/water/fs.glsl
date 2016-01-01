/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#version 330

layout(location = 0) out vec4 dif;

// World position of fragment
in vec3 world_pos;
in vec3 world_normal;

uniform vec3 light_dir;
uniform vec3 camera_pos;

uniform samplerCube envmap;

float fresnel(float n1, float n2, float angle)
{
  float r0 = (n1 - n2) / (n1 + n2);
  r0 = r0 * r0;

  return r0 + (1 - r0) * pow(1 - angle, 5.0);
}

void main()
{
  dif = vec4(.3, .5, .9, 1.0);

  vec3 view_dir = normalize(camera_pos - world_pos);

  float fr = fresnel(1.0, 1.33, dot(view_dir, world_normal));
  //fr = max(0.0, min(1.0, fr));

  vec3 halfway = normalize(light_dir + view_dir);

  float diffuse = max(dot(light_dir, world_normal), 0.0);
  float specular = 0.0;
  if(diffuse > 0.0)
  {
    specular = pow(max(dot(halfway, world_normal), 0.0), 4);
  }

  vec3 to_surface = normalize(world_pos - camera_pos);
  vec3 reflected_dir = reflect(to_surface, world_normal);
  //vec3 refracted_dir = refract(to_surface, world_normal, 1.0 / 1.3333);

  // TODO: Make shininess and friend parameters uniform data.
  float sunlight = pow(clamp(dot(reflected_dir, light_dir), 0.0, 1.0), 4);
  vec4 sunlight_color = sunlight * vec4(0.98, 0.83, 0.83, 1.0);

  vec4 reflected_color = texture(envmap, reflected_dir);
  //vec4 refracted_color = texture(envmap, refracted_dir);

  // Currently, we do not use any refracted color.

  dif = mix(diffuse * dif, reflected_color + sunlight_color, fr);

  // Let this commemorate the 7am session that *almost* rekted my sleep.
  //dif = reflected_color;
  //dif = mix(refracted_color, vec4((world_normal.xyz / 2.0) + 1.0, 1.0), 0.5);
  //dif = sunlight_color;
  //dif = vec4(reflected, 1.0);
  //dif = vec4(light_dir, 1.0);
  //dif = vec4(halfway, 1.0);
  //dif = vec4((normalize(view_dir) / 2.0) + 1.0, 1.0);
  //dif = vec4(dot(view_dir, vec3(0.0, 1.0, 0.0)), 0.0, 0.0, 1.0);
  //dif = vec4(dot(view_dir, world_normal), 0.0, 0.0, 1.0);
  //dif = vec4(fr, fr, fr, 1.0);
  //dif = vec4(world_normal_vec4.g, world_normal_vec4.g, world_normal_vec4.g, 1.0);
  //dif.rgb = vec3(1.0, 1.0, 1.0);
  //dif.rgb *= max(0.0, dot(light_dir, halfway));
  //dif.rgb *= mix(lambertian, specular, fr);
  //dif.rgb = world_normal;
  //dif = diffuse_intensity * dif;
  //dif = vec4(refracted_color.rgb, 1.0);
  //dif = vec4(normalize(world_pos - camera_pos) + .5, 1.0);
  //dif = vec4(refracted_dir + .5, 1.0);
  //dif = vec4(va_out.z, vb_out.z, 0.0, 1.0);
  //dif = vec4(world_normal.xyz, 1.0);
  //dif = specular * dif;
}

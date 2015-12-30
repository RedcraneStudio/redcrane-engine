/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#version 330

layout(location = 0) in vec3 world;

out vec3 world_pos;
out vec3 world_normal;

uniform float height;
uniform float disp;
uniform float time;

uniform mat4 projector;
uniform mat4 view;
uniform mat4 proj;

uniform int octaves;
uniform float amplitude;
uniform float frequency;
uniform float persistence;
uniform float lacunarity;

// From http://stackoverflow.com/questions/4200224/#4275343`
float height_fn(vec2 co)
{
  return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

float my_noise(vec2 co)
{
  float ret = 0.0;

  float cur_freq = frequency;
  float cur_ampl = amplitude;
  for(int i = 0; i < octaves; ++i)
  {
    ret += height_fn(co * cur_freq) * cur_ampl;
    cur_freq *= persistence;
    cur_ampl *= lacunarity;
  }

  return min(max(ret, -1.0), 1.0);
}

vec3 line_plane_intersection(vec3 start, vec3 end)
{
  vec3 plane_normal = vec3(0.0, 1.0, 0.0);

  vec3 line = end - start;
  float t = -dot(plane_normal, start) / dot(plane_normal, line);

  return start + (t * end);
}

vec3 interceptPlane( vec3 source, vec3 dir, vec3 normal, float height )
{
  // Compute the distance between the source and the surface, following a ray,
  // then return the intersection
  // http://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S09/lectures/11_ray_tracing.pdf
  float distance = ( - height - dot( normal, source ) ) / dot( normal, dir );
  if( distance < 0.0 )
    return source + dir * distance;
  else
    return - ( vec3( source.x, height, source.z ) + vec3( dir.x, height, dir.z) * 200000.0 );
}

void main()
{
  // Unproject our grid to world space.
  //vec4 line_start = projector * vec4(uv.x, uv.y, 0.0, 1.0);
  //vec4 line_end = projector * vec4(uv.x, uv.y, 1.0, 1.0);

  //world_pos = interceptPlane(vec3(line_start), vec3(line_end - line_start),
                             //vec3(0.0, 1.0, 0.0), 0.0);
  //world_pos = line_plane_intersection(vec3(line_start.xyz / line_start.w),
                                      //vec3(line_end.xyz / line_end.w));
  //world_pos = vec3(line_end);
  //world_pos = vec3(line_start);

  world_pos = world;
  //world_pos.y += height_fn(world_pos.xz) * disp;

  // Sum some noise for height.
  //world_pos.y += my_noise(uv) * disp;

  // Generate normals
  //vec4 before1 = vec4(world_pos.x + 1.0, height, world_pos.y,       1.0);
  //before1.y += my_noise(before1.xz) * disp;
  //vec4 before2 = vec4(world_pos.x,       height, world_pos.y + 1.0, 1.0);
  //before2.y += my_noise(before2.xz) * disp;

  //world_normal = vec3(normalize(cross(vec3(before1) - world_pos, vec3(before2) - world_pos)));
  world_normal = vec3(0.0, 1.0, 0.0);

  gl_Position = proj * view * vec4(world_pos, 1.0);
}

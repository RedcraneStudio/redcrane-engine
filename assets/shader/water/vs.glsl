/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#version 330

layout(location = 0) in vec2 uv;

out vec3 world_pos;
out vec3 world_normal;

uniform vec4 plane;
uniform float time;

uniform mat4 projector;
uniform mat4 view;
uniform mat4 proj;

uniform sampler2D heightmap;

uniform int octaves_in;
uniform float amplitude_in;
uniform float frequency_in;
uniform float persistence_in;
uniform float lacunarity_in;

vec3 intercept_plane(vec3 start, vec3 end)
{
  vec3 dir = end - start;
  float t = (plane.w - dot(plane.xyz, start)) / dot(plane.xyz, dir);

  return start + (t * dir);
}

// All from http://www.kamend.com/2012/06/perlin-noise-and-glsl/

vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 fade(vec2 t)
{
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec2 P)
{
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;

  vec4 i = permute(permute(ix) + iy);

  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  vec4 gy = abs(gx) - 0.5 ;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;

  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);

  vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;

  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));

  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}

// Also from the above source but I modified it a bit.

float fbm(vec2 P)
{
  float sum = 0.0;

  float cur_freq = frequency_in;
  float cur_ampl = amplitude_in;

  int i;

  for(i = 0; i < octaves_in; i+=1)
  {
      sum += cur_ampl * cnoise(P * cur_freq);
      cur_freq *= persistence_in;
      cur_ampl *= lacunarity_in;
  }
  return sum;
}

// My adapted code from someplace on stack overflow: see water/grid.cpp for the
// link.

float apply_sobel_kernel_x(vec2 pt, float grid, float scale)
{
  float s_2 = fbm(pt + vec2(+grid, +grid));
  float s_0 = fbm(pt + vec2(-grid, +grid));
  float s_5 = fbm(pt + vec2(+grid,  0.0));
  float s_3 = fbm(pt + vec2(-grid,  0.0));
  float s_8 = fbm(pt + vec2(+grid, -grid));
  float s_6 = fbm(pt + vec2(-grid, -grid));
  return scale * -(s_2 - s_0 + 2 * (s_5 - s_3) + s_8 - s_6);
}
float apply_sobel_kernel_y(vec2 pt, float grid, float scale)
{
  float s_6 = fbm(pt + vec2(-grid, -grid));
  float s_0 = fbm(pt + vec2(-grid, +grid));
  float s_7 = fbm(pt + vec2(  0.0, -grid));
  float s_1 = fbm(pt + vec2(  0.0, +grid));
  float s_8 = fbm(pt + vec2(+grid, -grid));
  float s_2 = fbm(pt + vec2(+grid, +grid));
  return scale * -(s_6 - s_0 + 2 * (s_7 - s_1) + s_8 - s_2);
}

void main()
{
  // Project our grid to world space

  vec4 start = projector * vec4(uv, -1.0, 1.0);
  vec4 end = projector * vec4(uv, +1.0, 1.0);

  world_pos = intercept_plane(vec3(start / start.w), vec3(end / end.w));

  // That 18 could be some sort of `velocity` parameter.
  vec2 coord = world_pos.xz + vec2(time, time) / 5.0;

  float cur = fbm(coord);
  world_pos.y += cur;

  world_normal.x = apply_sobel_kernel_x(coord, .01, 1.2);
  world_normal.z = apply_sobel_kernel_y(coord, .01, 1.2);
  world_normal.y = 1.0;
  world_normal = normalize(world_normal);
  //world_normal = vec3(0.0, 1.0, 0.0);

  gl_Position = proj * view * vec4(world_pos, 1.0);
}

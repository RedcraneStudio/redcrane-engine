/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#version 330 core

in vec2 uv;

out vec4 dst;

uniform sampler2D tex;

uniform vec3 fog_color;
uniform float fog_start;
uniform float fog_end;

void main()
{
  // Sample
  dst = texture(tex, uv);

  // Fog
  float fog_coord = gl_FragCoord.z / gl_FragCoord.w;
  float fog_factor = (fog_end - fog_coord) / (fog_end - fog_start);
  fog_factor = 1.0f - clamp(fog_factor, 0.0f, 1.0f);
  dst = mix(dst, vec4(fog_color, 1.0f), fog_factor);
}

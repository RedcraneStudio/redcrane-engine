/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#version 330
layout(location = 0) out vec4 frag_color;

in vec3 tex_coords;

uniform samplerCube envmap;

void main()
{
  frag_color = texture(envmap, tex_coords);
}


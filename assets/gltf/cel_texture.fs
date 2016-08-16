/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#version 330

in vec2 uv;

out vec4 color;

uniform sampler2D tex;

void main()
{
  color = texture(tex, uv);
}

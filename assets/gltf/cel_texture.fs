/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#version 330

in vec2 uv;

out vec4 color;

uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 emission;
uniform vec4 specular;
uniform float shininess;

uniform sampler2D tex;

void main()
{
  color = texture(tex, uv);
}

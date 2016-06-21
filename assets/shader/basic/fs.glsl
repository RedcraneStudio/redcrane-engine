#version 330

layout(location = 0) out vec4 diffuse_out;

in vec2 uv;
in vec4 world_pos;
in vec3 world_normal;

uniform vec3 light_pos;
//uniform vec4 dif;
uniform sampler2D diffuse;

float cel(float fin)
{
  if(fin < .1) { return .05f; }
  if(fin < .2) { return .15f; }
  if(fin < .3) { return .25f; }
  if(fin < .4) { return .35f; }
  if(fin < .5) { return .45f; }
  if(fin < .6) { return .55f; }
  if(fin < .7) { return .65f; }
  if(fin < .8) { return .75f; }
  if(fin < .9) { return .85f; }
  if(fin < 1.) { return .95f; }
}

void main()
{
  diffuse_out = texture(diffuse, uv);

  diffuse_out.x = cel(diffuse_out.x);
  diffuse_out.y = cel(diffuse_out.y);
  diffuse_out.z = cel(diffuse_out.z);

  //diffuse_out = dif;
  // diffuse_out.a = 1.0;

  /*
  float light_intensity = dot(normalize(world_normal),
                              normalize(light_pos - vec3(world_pos)));
  float ambient_intensity = .01;
  diffuse_out.rgb *= max(light_intensity, ambient_intensity);
  */
}

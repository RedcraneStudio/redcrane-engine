#version 330 core
layout(location = 0) out vec4 pos;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 color;

in vec3 position_cam;
in vec3 normal_cam;

uniform vec3 diffuse;
uniform float shininess;

void main()
{
  // Record the post-projection-space depth so we can easily write it to the
  // depth buffer later.
  pos = vec4(position_cam, gl_FragCoord.z);
  normal = vec4(normal_cam, gl_FragCoord.w);
  color = vec4(diffuse, shininess);
}

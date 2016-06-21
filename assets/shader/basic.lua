
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

version{"330"}

vs_input{location = 0, "vec3", "vertex"}
vs_input{location = 1, "vec3", "normal_in"}
vs_input{location = 2, "vec2", "uv_in"}

fs_output{"vec4", "diffuse_out"}

vs_fs_interface{"vec2", "uv"}
vs_fs_interface{"vec4", "world_pos"}
vs_fs_interface{"vec3", "world_normal"}

vs_uniform{"mat4", "projection", "proj"}
vs_uniform{"mat4", "view"}
vs_uniform{"mat4", "model"}

fs_uniform{"vec3", "light_pos"}
fs_uniform{"sampler2D", "diffuse", "dif"}

vs_code{[[
void main()
{
  // Calculate our world position and screen space position.
  world_pos = model * vec4(vertex, 1.0);
  gl_Position = proj * view * world_pos;

  uv = uv_in;

  // Calculate the vertex normal.
  world_normal = vec3(model * vec4(normal_in, 0.0));
}
]]}

fs_code{[[
void main()
{
  diffuse_out = texture(dif, uv);

  //float light_intensity = dot(normalize(world_normal),
  //                            normalize(light_pos - vec3(world_pos)));
  //float ambient_intensity = .01;
  //diffuse_out.rgb *= max(light_intensity, ambient_intensity);
}
]]}


-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

version{"330"}

vs_input{location = 0, "vec2", "vertex"}
vs_input{location = 1, "vec2", "uv_in"}

fs_output{"vec4", "diffuse_out"}

vs_fs_interface{"vec2", "uv"}

vs_uniform{"mat4", "projection", "ortho"}

fs_uniform{"sampler2D", "texture", "tex"}

vs_code{[[
void main()
{
  gl_Position = ortho * vec4(vertex, 0.0, 1.0);
  uv = uv_in;
}
]]}

fs_code{[[
void main()
{
  diffuse_out = texture(tex, uv);
}
]]}

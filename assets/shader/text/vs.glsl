#version 330

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec4 color;
layout(location = 3) in float ashift;
layout(location = 4) in float agamma;

// x: x, y: y, z: width, w: height
uniform vec4 viewport;
uniform vec2 translation;

out vec4 color_fs;
out vec2 tex_coord_fs;

void main() {
    color_fs = color;
    tex_coord_fs = tex_coord;

    gl_Position = vec4(translation + (vertex.xy / viewport.zw), vertex.z, 1.0f);
}

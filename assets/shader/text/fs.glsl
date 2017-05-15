#version 330

layout(location = 0) out vec4 dif;

in vec4 color_fs;
in vec2 tex_coord_fs;

uniform vec4 viewport;
uniform sampler2D atlas;

void main()
{
    float x = tex_coord_fs.x;
    float y = 1.0f - tex_coord_fs.y;
    float val = texture(atlas, vec2(x, y)).r;
    dif = color_fs * val;
}

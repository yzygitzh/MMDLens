#version 420 core

layout (binding = 0) uniform sampler2D tex_color;

in VS_OUT
{
    vec2 UV;
} fs_in;

void main() {
    gl_FragColor = texture(tex_color, fs_in.UV);
}

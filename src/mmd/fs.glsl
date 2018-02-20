#version 420 core

layout (binding = 0) uniform sampler2D tex_color;
layout (binding = 1) uniform sampler2D tex_normal;

in VS_OUT
{
    vec3 color;
} fs_in;

void main() {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

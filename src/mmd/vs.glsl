#version 420 core

uniform mat4 trans_matrix;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 UV;

out VS_OUT
{
    vec2 UV;
} vs_out;

void main() {
    gl_Position = proj_matrix * mv_matrix * trans_matrix * vec4(pos, 1.0);
    vs_out.UV = UV;
}

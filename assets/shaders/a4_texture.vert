#version 430
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTcoord;
layout (location = 2) in vec3 vNorm;

uniform mat4 u_mvp;

out vec2 uv;

void main()
{
    uv = vTcoord;
    gl_Position = u_mvp * vec4(vPos, 1.0);
}

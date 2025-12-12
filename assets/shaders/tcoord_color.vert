#version 430
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTcoord;
layout (location = 2) in vec3 vNorm;

out vec3 color;

uniform mat4 u_mvp;

void main()
{
    vec4 pos = vec4(vPos, 1.0);
    color = vec3(vTcoord, 0.0);
    gl_Position = u_mvp * pos;
}

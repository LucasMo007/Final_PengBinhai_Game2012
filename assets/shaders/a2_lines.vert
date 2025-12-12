#version 430
layout (location = 0) in vec2 vPos;

uniform mat4 u_mvp;     // world * view * proj

void main()
{
    vec4 pos = vec4(vPos, 0.0, 1.0);
    gl_Position = u_mvp * pos;
}

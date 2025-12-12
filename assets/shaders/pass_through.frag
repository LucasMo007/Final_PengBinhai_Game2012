#version 430
in vec3 color;
out vec4 fragColor;

uniform vec3 u_color;

void main()
{
    fragColor = vec4(u_color, 1.0);
}

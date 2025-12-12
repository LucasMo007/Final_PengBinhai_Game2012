#version 430
out vec4 fragColor;
in vec2 uv;

uniform sampler2D u_sampler0;

void main()
{
    vec4 tex_col = texture(u_sampler0, uv);
    fragColor = vec4(tex_col);
}

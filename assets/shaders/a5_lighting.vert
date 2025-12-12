#version 330 core


layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_world;
uniform mat4 u_mvp;

out vec3 v_world_pos;
out vec3 v_world_normal;

void main()
{
    
    vec4 world_pos   = u_world * vec4(a_position, 1.0);
    v_world_pos      = world_pos.xyz;

    
    mat3 normal_mat  = mat3(transpose(inverse(u_world)));
    v_world_normal   = normalize(normal_mat * a_normal);

  
    gl_Position      = u_mvp * vec4(a_position, 1.0);
}


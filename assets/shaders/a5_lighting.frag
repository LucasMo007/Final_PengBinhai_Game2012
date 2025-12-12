#version 330 core

in vec3 v_world_pos;
in vec3 v_world_normal;

out vec4 out_color;

// ----------------------
// 相机参数
// ----------------------
uniform vec3 u_view_position;

// ----------------------
// 方向光 Directional Light
// ----------------------
uniform vec3 u_dir_light_direction;
uniform vec3 u_dir_light_color;

// ----------------------
// 聚光灯 Spot Light
// ----------------------
uniform vec3 u_spot_light_position;
uniform vec3 u_spot_light_direction;
uniform vec3 u_spot_light_color;
uniform float u_spot_light_inner_cutoff;  
uniform float u_spot_light_outer_cutoff;

// ----------------------
// 点光源 Point Light (带衰减)
// ----------------------
uniform vec3 u_point_light_position;
uniform vec3 u_point_light_color;
uniform float u_point_light_constant;
uniform float u_point_light_linear;
uniform float u_point_light_quadratic;

// ----------------------
// 材质参数 Material
// ----------------------
uniform vec3 u_material_ambient;
uniform vec3 u_material_diffuse;
uniform vec3 u_material_specular;
uniform float u_material_shininess;

void main()
{
    vec3 N = normalize(v_world_normal);
    vec3 V = normalize(u_view_position - v_world_pos);

    // ==========================================
    // 1. 方向光 Directional Light
    // ==========================================
    vec3 L_dir = normalize(-u_dir_light_direction);
    vec3 R_dir = reflect(-L_dir, N);

    // Ambient
    vec3 ambient = u_material_ambient * u_dir_light_color;

    // Diffuse
    float diff_dir = max(dot(N, L_dir), 0.0);
    vec3 diffuse_dir = diff_dir * u_material_diffuse * u_dir_light_color;

    // Specular
    float spec_dir = 0.0;
    if (diff_dir > 0.0) {
        spec_dir = pow(max(dot(R_dir, V), 0.0), u_material_shininess);
    }
    vec3 specular_dir = spec_dir * u_material_specular * u_dir_light_color;

    // ==========================================
    // 2. 聚光灯 Spot Light
    // ==========================================
    vec3 L_spot = normalize(u_spot_light_position - v_world_pos);
    vec3 R_spot = reflect(-L_spot, N);

    float theta = dot(L_spot, normalize(-u_spot_light_direction));
    float epsilon = u_spot_light_inner_cutoff - u_spot_light_outer_cutoff;
    float intensity = clamp((theta - u_spot_light_outer_cutoff) / epsilon, 0.0, 1.0);

    float diff_spot = max(dot(N, L_spot), 0.0);
    vec3 diffuse_spot = diff_spot * u_material_diffuse * u_spot_light_color * intensity;

    float spec_spot = 0.0;
    if (diff_spot > 0.0) {
        spec_spot = pow(max(dot(R_spot, V), 0.0), u_material_shininess);
    }
    vec3 specular_spot = spec_spot * u_material_specular * u_spot_light_color * intensity;

    // ==========================================
    // 3. 点光源 Point Light (衰减)
    // ==========================================
    vec3 L_point = normalize(u_point_light_position - v_world_pos);
    vec3 R_point = reflect(-L_point, N);

    float distance = length(u_point_light_position - v_world_pos);
    float attenuation = 1.0 /
        (u_point_light_constant +
         u_point_light_linear * distance +
         u_point_light_quadratic * distance * distance);

    float diff_point = max(dot(N, L_point), 0.0);
    vec3 diffuse_point = diff_point * u_material_diffuse * u_point_light_color * attenuation;

    float spec_point = 0.0;
    if (diff_point > 0.0) {
        spec_point = pow(max(dot(R_point, V), 0.0), u_material_shininess);
    }
    vec3 specular_point = spec_point * u_material_specular * u_point_light_color * attenuation;

    // ==========================================
    // 4. 合并所有光照 Final Color
    // ==========================================
    vec3 color = ambient +
                 diffuse_dir + specular_dir +
                 diffuse_spot + specular_spot +
                 diffuse_point + specular_point;

    out_color = vec4(color, 1.0);
}

#include "Window.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"

#include <imgui/imgui.h>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <ctime>

enum ShaderType
{
    SHADER_LIGHTING,
    SHADER_FLAT,
    SHADER_SAMPLE_TEXTURE,
    SHADER_POSITION_COLOR,
    SHADER_TCOORD_COLOR,
    SHADER_NORMAL_COLOR,
    SHADER_TYPE_COUNT
};

enum MeshType
{
    MESH_PLANE,
    MESH_SPHERE,
    MESH_HEMISPHERE,
    MESH_CAR,
    MESH_TYPE_COUNT
};

enum TextureType
{
    TEXTURE_CT4,
    TEXTURE_WHITE,
    TEXTURE_GRADIENT_WARM,
    TEXTURE_GRADIENT_COOL,
    TEXTURE_TYPE_COUNT
};

void LoadTextures(Texture textures[TEXTURE_TYPE_COUNT])
{
    Image ct4, white, warm, cool;

    LoadImage(&ct4, "./assets/textures/ct4_orange.bmp");
    LoadImage(&white, 1, 1);
    LoadImage(&warm, 512, 512);
    LoadImage(&cool, 512, 512);

    white.pixels[0] = { 0xFF, 0xFF, 0xFF, 0xFF };
    LoadImageGradient(&warm, Vector3Zeros, Vector3UnitX, Vector3UnitY, Vector3UnitX + Vector3UnitY);
    LoadImageGradient(&cool, Vector3UnitZ, Vector3UnitZ + Vector3UnitX, Vector3UnitY + Vector3UnitZ, Vector3Ones);

    LoadTexture(&textures[TEXTURE_CT4], ct4);
    LoadTexture(&textures[TEXTURE_WHITE], white);
    LoadTexture(&textures[TEXTURE_GRADIENT_WARM], warm);
    LoadTexture(&textures[TEXTURE_GRADIENT_COOL], cool);
}

struct Camera
{
    float pitch = 0.0f;
    float yaw = 0.0f;
    Vector3 position = Vector3Zeros;
};

int main()
{
    CreateWindow(800, 800, "Graphics 1 - First Person Camera");

    Mesh meshes[MESH_TYPE_COUNT];
    LoadMeshPlane(&meshes[MESH_PLANE]);
    LoadMeshSphere(&meshes[MESH_SPHERE]);
    LoadMeshHemisphere(&meshes[MESH_HEMISPHERE]);
    LoadMeshObj(&meshes[MESH_CAR], "./assets/meshes/ct4.obj");

    GLuint position_color_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/position_color.vert");
    GLuint tcoord_color_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/tcoord_color.vert");
    GLuint normal_color_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/normal_color.vert");
    GLuint vertex_color_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/vertex_color.frag");
    GLuint a4_texture_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/a4_texture.vert");
    GLuint a4_texture_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/a4_texture.frag");
    GLuint a5_lighting_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/a5_lighting.vert");
    GLuint a5_lighting_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/a5_lighting.frag");
    GLuint pass_through_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/pass_through.vert");
    GLuint pass_through_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/pass_through.frag");

    GLuint shaders[SHADER_TYPE_COUNT];
    shaders[SHADER_LIGHTING] = CreateProgram(a5_lighting_vert, a5_lighting_frag);
    shaders[SHADER_FLAT] = CreateProgram(pass_through_vert, pass_through_frag);
    shaders[SHADER_SAMPLE_TEXTURE] = CreateProgram(a4_texture_vert, a4_texture_frag);
    shaders[SHADER_POSITION_COLOR] = CreateProgram(position_color_vert, vertex_color_frag);
    shaders[SHADER_TCOORD_COLOR] = CreateProgram(tcoord_color_vert, vertex_color_frag);
    shaders[SHADER_NORMAL_COLOR] = CreateProgram(normal_color_vert, vertex_color_frag);

    Texture textures[TEXTURE_TYPE_COUNT];
    LoadTextures(textures);

    Camera camera;
    camera.position = { 0.0f, 5.0f, 10.0f };

    Vector3 light_position = { 0.0f, 5.0f, 0.0f };
    Vector3 light_color = Vector3Ones;

    Vector3 dir_light_direction = Vector3Normalize({ -1.0f, -1.0f, -1.0f });
    Vector3 dir_light_color = Vector3Ones;

    Vector3 spot_light_position = { 0.0f, 5.0f, 0.0f };
    Vector3 spot_light_direction = { 0.0f, -1.0f, 0.0f };
    Vector3 spot_light_color = Vector3Ones;
    float spot_light_inner_cutoff = 12.5f * DEG2RAD;
    float spot_light_outer_cutoff = 17.5f * DEG2RAD;

    Vector3 point_light_color = { 1.0f, 0.8f, 0.2f };
    float point_light_constant = 1.0f;
    float point_light_linear = 0.09f;
    float point_light_quadratic = 0.032f;
    float point_light_radius = 10.0f;
    float point_light_angle = 0.0f;

    int shader_index = SHADER_LIGHTING;
    int mesh_index = MESH_CAR;
    int texture_index = TEXTURE_CT4;

    float mouse_sensitivity = 0.002f;

    SetCursorMode(CURSOR_DISABLED);

    while (!WindowShouldClose())
    {
        BeginFrame();
        float dt = FrameTime();

        if (IsKeyPressed(KEY_ESCAPE))
            SetWindowShouldClose(true);

        if (IsKeyPressed(KEY_GRAVE_ACCENT))
            ++shader_index %= SHADER_TYPE_COUNT;

        if (IsKeyPressed(KEY_TAB))
            mesh_index = (mesh_index + 1) % MESH_TYPE_COUNT;

        if (IsKeyPressed(KEY_T))
            ++texture_index %= TEXTURE_TYPE_COUNT;

        point_light_angle += dt * 0.5f;
        Vector3 point_light_position = {
            point_light_radius * cos(point_light_angle),
            5.0f,
            point_light_radius * sin(point_light_angle)
        };

        double mouse_dx, mouse_dy;
        GetMouseDelta(&mouse_dx, &mouse_dy);

        camera.yaw -= mouse_dx * mouse_sensitivity;
        camera.pitch -= mouse_dy * mouse_sensitivity;

        const float max_pitch = 89.0f * DEG2RAD;
        camera.pitch = Clamp(camera.pitch, -max_pitch, max_pitch);

        Matrix camera_rotation = MatrixRotateY(camera.yaw) * MatrixRotateX(camera.pitch);
        Vector3 camera_forward = { camera_rotation.m8, camera_rotation.m9, camera_rotation.m10 };
        Vector3 camera_right = { camera_rotation.m0, camera_rotation.m1, camera_rotation.m2 };
        Vector3 camera_up = { camera_rotation.m4, camera_rotation.m5, camera_rotation.m6 };

        float move_speed = 10.0f;

        if (IsKeyDown(KEY_W))
            camera.position -= camera_forward * move_speed * dt;
        if (IsKeyDown(KEY_S))
            camera.position += camera_forward * move_speed * dt;
        if (IsKeyDown(KEY_A))
            camera.position -= camera_right * move_speed * dt;
        if (IsKeyDown(KEY_D))
            camera.position += camera_right * move_speed * dt;
        if (IsKeyDown(KEY_SPACE))
            camera.position += camera_up * move_speed * dt;
        if (IsKeyDown(KEY_LEFT_SHIFT))
            camera.position -= camera_up * move_speed * dt;

        Matrix proj = MatrixPerspective(75.0f * DEG2RAD,
            WindowWidth() / (float)WindowHeight(), 0.01f, 100.0f);

        Matrix view = MatrixInvert(camera_rotation *
            MatrixTranslate(camera.position.x, camera.position.y, camera.position.z));

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            Matrix groundWorld =
                MatrixTranslate(0.0f, 0.0f, 0.0f) *
                MatrixRotateX(-PI * 0.5f) *
                MatrixScale(100.0f, 1.0f, 100.0f);

            Matrix groundMvp = groundWorld * view * proj;

            BeginShader(shaders[SHADER_FLAT]);
            SendVec3({ 0.3f, 0.3f, 0.3f }, "u_color");
            SendMat4(groundMvp, "u_mvp");
            DrawMesh(meshes[MESH_PLANE]);
            EndShader();
        }

        {
            Matrix world = MatrixScale(0.1f, 0.1f, 0.1f);
            Matrix mvp = world * view * proj;

            BeginTexture(textures[texture_index]);
            BeginShader(shaders[shader_index]);

            SendMat4(world, "u_world");
            SendMat4(mvp, "u_mvp");
            SendVec3(light_position, "u_light_position");
            SendVec3(light_color, "u_light_color");

            if (shader_index == SHADER_LIGHTING)
            {
                SendVec3(dir_light_direction, "u_dir_light_direction");
                SendVec3(dir_light_color, "u_dir_light_color");
                SendVec3(camera.position, "u_view_position");

                SendVec3({ 0.1f, 0.1f, 0.1f }, "u_material_ambient");
                SendVec3({ 1.0f, 0.7f, 0.3f }, "u_material_diffuse");
                SendVec3({ 1.0f, 1.0f, 1.0f }, "u_material_specular");
                SendFloat(32.0f, "u_material_shininess");

                SendVec3(spot_light_position, "u_spot_light_position");
                SendVec3(spot_light_direction, "u_spot_light_direction");
                SendVec3(spot_light_color, "u_spot_light_color");
                SendFloat(cos(spot_light_inner_cutoff), "u_spot_light_inner_cutoff");
                SendFloat(cos(spot_light_outer_cutoff), "u_spot_light_outer_cutoff");

                SendVec3(point_light_position, "u_point_light_position");
                SendVec3(point_light_color, "u_point_light_color");
                SendFloat(point_light_constant, "u_point_light_constant");
                SendFloat(point_light_linear, "u_point_light_linear");
                SendFloat(point_light_quadratic, "u_point_light_quadratic");
            }

            DrawMesh(meshes[mesh_index]);
            EndShader();
            EndTexture();
        }

        {
            Matrix world = MatrixTranslate(light_position.x, light_position.y, light_position.z);
            Matrix mvp = world * view * proj;

            BeginShader(shaders[SHADER_FLAT]);
            SendVec3(light_color, "u_color");
            SendMat4(mvp, "u_mvp");
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            DrawMesh(meshes[MESH_SPHERE]);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            EndShader();
        }

        {
            Matrix world = MatrixTranslate(point_light_position.x,
                point_light_position.y,
                point_light_position.z) *
                MatrixScale(0.3f, 0.3f, 0.3f);
            Matrix mvp = world * view * proj;

            BeginShader(shaders[SHADER_FLAT]);
            SendVec3(point_light_color, "u_color");
            SendMat4(mvp, "u_mvp");
            DrawMesh(meshes[MESH_SPHERE]);
            EndShader();
        }

        BeginGui();

        ImGui::SetNextWindowSize(ImVec2(260, 190), ImGuiCond_Always);
        ImGui::Begin("Controls");
        ImGui::Text("Mouse   : Look around");
        ImGui::Text("W / S   : Move forward / backward");
        ImGui::Text("A / D   : Strafe left / right");
        ImGui::Text("Space   : Move up");
        ImGui::Text("Shift   : Move down");
        ImGui::Separator();
        ImGui::Text("`       : Change shader");
        ImGui::Text("Tab     : Change mesh");
        ImGui::Text("T       : Change texture");

        ImGui::End();

        EndGui();


     
        Loop();
        EndFrame();
    }

    for (int i = 0; i < TEXTURE_TYPE_COUNT; i++)
        UnloadTexture(&textures[i]);

    for (int i = 0; i < SHADER_TYPE_COUNT; i++)
        DestroyProgram(&shaders[i]);

    for (int i = 0; i < MESH_TYPE_COUNT; i++)
        UnloadMesh(&meshes[i]);

    DestroyWindow();
    return 0;
}

#include "Mesh.h"
#include "Buffer.h"
#include <cstdio>
#include <cassert>

#define PAR_SHAPES_IMPLEMENTATION
#include <par_shapes/par_shapes.h>

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj/fast_obj.h>

void LoadMeshGPU(Mesh* mesh);
void LoadMeshPar(Mesh* mesh, par_shapes_mesh* par);
void LoadMeshPlaneOptimal(Mesh* mesh);
void LoadMeshPlaneUnoptimal(Mesh* mesh);


void LoadMeshObj(Mesh* mesh, const char* path)
{
    fastObjMesh* obj = fast_obj_read(path);


    if (obj == nullptr)
    {
        printf("ERROR: Failed to load OBJ file: %s\n", path);
      
        mesh->vertex_count = 0;
        return;
    }

    size_t vc = obj->index_count;
    mesh->vertex_count = vc;
    mesh->positions.resize(vc);
    mesh->normals.resize(vc);

    mesh->tcoords.resize(vc);

    if (obj->texcoord_count > 0)
    {
        mesh->tcoords.resize(vc);
    }

    Vector3* positions = (Vector3*)obj->positions;
    Vector3* normals = (Vector3*)obj->normals;
    Vector2* texcoords = (Vector2*)obj->texcoords;

    for (size_t i = 0; i < vc; i++)
    {
        fastObjUInt idx_v = obj->indices[i].p;
        fastObjUInt idx_vn = obj->indices[i].n;
        fastObjUInt idx_vt = obj->indices[i].t;

        mesh->positions[i] = positions[idx_v];
        mesh->normals[i] = normals[idx_vn];
   

        if (obj->texcoord_count > 0 && idx_vt < obj->texcoord_count)
        {
            mesh->tcoords[i] = texcoords[idx_vt];
        }
    }

    fast_obj_destroy(obj);
    LoadMeshGPU(mesh);
}
void UnloadMesh(Mesh* mesh)
{
    DestroyVertexArray(&mesh->vao);
    DestroyBuffer(&mesh->pbo);
    DestroyBuffer(&mesh->tbo);
    DestroyBuffer(&mesh->nbo);
    DestroyBuffer(&mesh->ibo);

    mesh->positions.resize(0);
    mesh->tcoords.resize(0);
    mesh->normals.resize(0);
    mesh->indices.resize(0);

    mesh->vertex_count = -1;
}

void LoadMeshPlane(Mesh* mesh)
{
   

    LoadMeshPlaneOptimal(mesh);
    LoadMeshGPU(mesh);
}

void LoadMeshSphere(Mesh* mesh)
{
    par_shapes_mesh* par = par_shapes_create_parametric_sphere(8, 8);
    LoadMeshPar(mesh, par);
    par_shapes_free_mesh(par);

    LoadMeshGPU(mesh);
}

void LoadMeshHemisphere(Mesh* mesh)
{
    par_shapes_mesh* par = par_shapes_create_hemisphere(4, 4);
    LoadMeshPar(mesh, par);
    par_shapes_free_mesh(par);

    LoadMeshGPU(mesh);
}

void LoadMeshTetrahedron(Mesh* mesh)
{
    par_shapes_mesh* par = par_shapes_create_tetrahedron();
    LoadMeshPar(mesh, par);
    par_shapes_free_mesh(par);

    LoadMeshGPU(mesh);
}

void LoadMeshCube(Mesh* mesh)
{
    par_shapes_mesh* par = par_shapes_create_cube();
    par_shapes_translate(par, -0.5f, -0.5f, -0.5f);

    LoadMeshPar(mesh, par);
    par_shapes_free_mesh(par);

    LoadMeshGPU(mesh);
}

void LoadMeshOctahedron(Mesh* mesh)
{
    par_shapes_mesh* par = par_shapes_create_octahedron();
    LoadMeshPar(mesh, par);
    par_shapes_free_mesh(par);

    LoadMeshGPU(mesh);
}

void LoadMeshDodecahedron(Mesh* mesh)
{
    par_shapes_mesh* par = par_shapes_create_dodecahedron();
    LoadMeshPar(mesh, par);
    par_shapes_free_mesh(par);

    LoadMeshGPU(mesh);
}

void LoadMeshIcosahedron(Mesh* mesh)
{
    par_shapes_mesh* par = par_shapes_create_icosahedron();
    LoadMeshPar(mesh, par);
    par_shapes_free_mesh(par);

    LoadMeshGPU(mesh);
}

void DrawMesh(const Mesh& mesh)
{
    BindVertexArray(mesh.vao);
    if (mesh.ibo != GL_NONE)
        glDrawElements(GL_TRIANGLES, mesh.vertex_count, GL_UNSIGNED_SHORT, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertex_count);
    UnbindVertexArray(mesh.vao);
}

void LoadMeshGPU(Mesh* mesh)
{
    assert(!mesh->positions.empty());
    mesh->pbo = CreateBuffer();
    BindVertexBuffer(mesh->pbo);
    UpdateVertexBuffer(mesh->positions.data(), mesh->positions.size() * sizeof(Vector3));
    UnbindVertexBuffer(mesh->pbo);

    if (!mesh->tcoords.empty())
    {
        mesh->tbo = CreateBuffer();
        BindVertexBuffer(mesh->tbo);
        UpdateVertexBuffer(mesh->tcoords.data(), mesh->tcoords.size() * sizeof(Vector2));
        UnbindVertexBuffer(mesh->tbo);
    }
    else
        printf("Warning: mesh loaded without texture coordinates\n");

    if (!mesh->normals.empty())
    {
        mesh->nbo = CreateBuffer();
        BindVertexBuffer(mesh->nbo);
        UpdateVertexBuffer(mesh->normals.data(), mesh->normals.size() * sizeof(Vector3));
        UnbindVertexBuffer(mesh->nbo);
    }
    else
        printf("Warning: mesh loaded without normals\n");

    if (!mesh->indices.empty())
    {
        mesh->ibo = CreateBuffer();
        BindIndexBuffer(mesh->ibo);
            UpdateElementBuffer(mesh->indices.data(), mesh->indices.size() * sizeof(uint16_t));
        UnbindIndexBuffer(mesh->ibo);
    }
    else
        printf("Warning: mesh loaded without index buffer\n");

    mesh->vao = CreateVertexArray();
    BindVertexArray(mesh->vao);

    if (mesh->ibo != GL_NONE)
        BindIndexBuffer(mesh->ibo);

    EnableVertexAttribute(0);
    EnableVertexAttribute(1);
    EnableVertexAttribute(2);

    assert(mesh->pbo != GL_NONE);
    BindVertexBuffer(mesh->pbo);
    SetVertexAttribute(0, 3, GL_FLOAT, sizeof(Vector3));
    UnbindVertexBuffer(mesh->pbo);

    if (mesh->tbo != GL_NONE)
    {
        BindVertexBuffer(mesh->tbo);
        SetVertexAttribute(1, 2, GL_FLOAT, sizeof(Vector2));
        UnbindVertexBuffer(mesh->tbo);
    }
    
    if (mesh->nbo != GL_NONE)
    {
        BindVertexBuffer(mesh->nbo);
        SetVertexAttribute(2, 3, GL_FLOAT, sizeof(Vector3));
        UnbindVertexBuffer(mesh->nbo);
    }

    UnbindVertexArray(mesh->vao);

    if (mesh->ibo != GL_NONE)
        UnbindIndexBuffer(mesh->ibo);
}

void LoadMeshPar(Mesh* mesh, par_shapes_mesh* par)
{
  

    mesh->vertex_count = par->ntriangles * 3;
    mesh->indices.resize(mesh->vertex_count);
    mesh->positions.resize(par->npoints);
    mesh->normals.resize(par->npoints);
    if (par->tcoords != nullptr)
        mesh->tcoords.resize(par->npoints);

    Vector3* par_positions = reinterpret_cast<Vector3*>(par->points);
    Vector3* par_normals = reinterpret_cast<Vector3*>(par->normals);
    Vector2* par_tcoords = reinterpret_cast<Vector2*>(par->tcoords);
    memcpy(mesh->positions.data(), par_positions, par->npoints * sizeof(Vector3));
    memcpy(mesh->normals.data(), par_normals, par->npoints * sizeof(Vector3));
    memcpy(mesh->indices.data(), par->triangles, mesh->vertex_count * sizeof(PAR_SHAPES_T));
    if (par_tcoords != nullptr)
        memcpy(mesh->tcoords.data(), par_tcoords, par->npoints * sizeof(Vector2));
}

void LoadMeshPlaneOptimal(Mesh* mesh)
{
    mesh->vertex_count = 6;

    mesh->positions.resize(4);
    mesh->tcoords.resize(4);
    mesh->normals.resize(4);
    mesh->indices.resize(6);

    mesh->positions[0] = { -0.5f, -0.5f, 0.0f };
    mesh->positions[1] = { 0.5f, -0.5f, 0.0f };
    mesh->positions[2] = { 0.5f,  0.5f, 0.0f };
    mesh->positions[3] = { -0.5f, 0.5f, 0.0f };


    mesh->tcoords[0] = { 0.0f, 0.0f };
    mesh->tcoords[1] = { 1.0f, 0.0f };
    mesh->tcoords[2] = { 1.0f, 1.0f };
    mesh->tcoords[3] = { 0.0f, 1.0f };

  

    mesh->indices[0] = 0;
    mesh->indices[1] = 1;
    mesh->indices[2] = 2;
    mesh->indices[3] = 0;
    mesh->indices[4] = 2;
    mesh->indices[5] = 3;
}

void LoadMeshPlaneUnoptimal(Mesh* mesh)
{
    mesh->vertex_count = 6;

    std::vector<Vector3> positions;
    std::vector<Vector2> tcoords;
    std::vector<Vector3> normals;
    std::vector<uint16_t> indices;

    positions.resize(4);
    tcoords.resize(4);
    normals.resize(4);
    indices.resize(6);
    
    positions[0] = { -0.5f, -0.5f, 0.0f };
    positions[1] = {  0.5f, -0.5f, 0.0f };
    positions[2] = {  0.5f,  0.5f, 0.0f };
    positions[3] = { -0.5f,  0.5f, 0.0f };

    tcoords[0] = { 0.0f, 0.0f };
    tcoords[1] = { 1.0f, 0.0f };
    tcoords[2] = { 1.0f, 1.0f };
    tcoords[3] = { 0.0f, 1.0f };

    normals[0] = Vector3UnitZ;
    normals[1] = Vector3UnitZ;
    normals[2] = Vector3UnitZ;
    normals[3] = Vector3UnitZ;

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    mesh->positions.resize(6);
    mesh->tcoords.resize(6);
    mesh->normals.resize(6);
    for (size_t i = 0; i < indices.size(); i++)
    {
        uint16_t index = indices[i];
        Vector3 v = positions[index];
        Vector2 vt = tcoords[index];
        Vector3 vn = normals[index];

        mesh->positions[i] = v;
        mesh->tcoords[i] = vt;
        mesh->normals[i] = vn;
    }
}

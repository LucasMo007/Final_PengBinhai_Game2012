#pragma once
#include <cstdint>
#include <glad/glad.h>
#include <vector>
#include "raymath.h"

struct Mesh
{
	std::vector<Vector3> positions;
	std::vector<Vector2> tcoords;
	std::vector<Vector3> normals;
	std::vector<uint16_t> indices;

	GLuint pbo = GL_NONE;	// positions buffer
	GLuint tbo = GL_NONE;	// tcoords buffer
	GLuint nbo = GL_NONE;	// normals buffer
	GLuint ibo = GL_NONE;	// index buffer

	GLuint vao = GL_NONE;
	int vertex_count = -1;
};

void UnloadMesh(Mesh* mesh);

void LoadMeshTetrahedron(Mesh* mesh);
void LoadMeshCube(Mesh* mesh);
void LoadMeshOctahedron(Mesh* mesh);
void LoadMeshDodecahedron(Mesh* mesh);
void LoadMeshIcosahedron(Mesh* mesh);

void LoadMeshPlane(Mesh* mesh);
void LoadMeshSphere(Mesh* mesh);
void LoadMeshHemisphere(Mesh* mesh);

void LoadMeshObj(Mesh* mesh, const char* path);

void DrawMesh(const Mesh& mesh);
#pragma once
#include <glad/glad.h>
#include "raymath.h"

GLuint CreateVertexArray();
GLuint CreateBuffer();

void DestroyVertexArray(GLuint* vao);
void DestroyBuffer(GLuint* vbo);

void BindVertexArray(GLuint vao);
void UnbindVertexArray(GLuint vao);

void BindVertexBuffer(GLuint vbo);
void UnbindVertexBuffer(GLuint vbo);

void BindIndexBuffer(GLuint ebo);
void UnbindIndexBuffer(GLuint ebo);

void EnableVertexAttribute(GLuint index);
void DisableVertexAttribute(GLuint index);

void SetVertexAttribute(GLuint index, GLint compSize, GLenum type, GLsizei stride);

void UpdateVertexBuffer(void* data, int data_size);
void UpdateElementBuffer(void* data, int data_size);
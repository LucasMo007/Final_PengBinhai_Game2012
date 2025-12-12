#pragma once
#include <glad/glad.h>
#include "raymath.h"

GLuint CreateShader(GLint type, const char* path);
void DestroyShader(GLuint* handle);

GLuint CreateProgram(GLuint vs, GLuint fs);
void DestroyProgram(GLuint* handle);

void BeginShader(GLuint shader);
void EndShader();

void SendInt(int value, const char* name);
void SendFloat(float value, const char* name);

void SendVec2(Vector2 value, const char* name);
void SendVec3(Vector3 value, const char* name);
void SendVec4(Vector4 value, const char* name);

void SendMat3(Matrix value, const char* name);
void SendMat4(Matrix value, const char* name);

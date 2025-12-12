#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

static GLuint f_shader = GL_NONE;

int GetUniformLocation(GLuint shader, const char* name);

GLuint CreateShader(GLint type, const char* path)
{
    GLuint shader = 0;
    try
    {
        // Load text file
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path);

        // Interpret the file as a giant string
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        // Verify shader type matches shader file extension
        const char* ext = strrchr(path, '.');
        switch (type)
        {
        case GL_VERTEX_SHADER:
            assert(strcmp(ext, ".vert") == 0);
            break;

        case GL_FRAGMENT_SHADER:
            assert(strcmp(ext, ".frag") == 0);
            break;
        default:
            assert(false, "Invalid shader type");
            break;
        }

        // Compile text as a shader
        std::string str = stream.str();
        const char* src = str.c_str();
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        // Check for compilation errors
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "Shader failed to compile: \n" << infoLog << std::endl;
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "Shader (" << path << ") not found: " << e.what() << std::endl;
    }

    return shader;
}

void DestroyShader(GLuint* handle)
{
    assert(*handle != GL_NONE);
    glDeleteShader(*handle);
    *handle = GL_NONE;
}

GLuint CreateProgram(GLuint vs, GLuint fs)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        program = GL_NONE;
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    return program;
}

void DestroyProgram(GLuint* handle)
{
    assert(*handle != GL_NONE);
    glDeleteProgram(*handle);
    *handle = GL_NONE;
}

void BeginShader(GLuint shader)
{
    assert(f_shader == GL_NONE);
    glUseProgram(shader);
    f_shader = shader;
}

void EndShader()
{
    assert(f_shader != GL_NONE);
    glUseProgram(GL_NONE);
    f_shader = GL_NONE;
}

void SendInt(int value, const char* name)
{
    int location = GetUniformLocation(f_shader, name);
    glUniform1i(location, value);
}

void SendFloat(float value, const char* name)
{
    int location = GetUniformLocation(f_shader, name);
    glUniform1f(location, value);
}

void SendVec2(Vector2 value, const char* name)
{
    int location = GetUniformLocation(f_shader, name);
    glUniform2f(location, value.x, value.y);
}

void SendVec3(Vector3 value, const char* name)
{
    int location = GetUniformLocation(f_shader, name);
    glUniform3f(location, value.x, value.y, value.z);
}

void SendVec4(Vector4 value, const char* name)
{
    int location = GetUniformLocation(f_shader, name);
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void SendMat3(Matrix v, const char* name)
{
    float arr[9] =
    {
        v.m0, v.m1, v.m2,
        v.m4, v.m5, v.m6,
        v.m8, v.m9, v.m10
    };

    int location = GetUniformLocation(f_shader, name);
    glUniformMatrix3fv(location, 1, GL_FALSE, arr);
}

void SendMat4(Matrix value, const char* name)
{
    int location = GetUniformLocation(f_shader, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, MatrixToFloat(value));
}

int GetUniformLocation(GLuint shader, const char* name)
{
    GLint location = glGetUniformLocation(shader, name);
    if (location == -1)
    {
        printf("Warning: shader %i failed to send uniform %s\n", shader, name);
        //assert(false); <-- eventually we might send data to shaders that don't use it
    }
    return location;
}

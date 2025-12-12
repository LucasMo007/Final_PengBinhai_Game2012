#include "Buffer.h"
#include <cassert>

static GLuint f_vao = GL_NONE;
static GLuint f_vbo = GL_NONE;
static GLuint f_ibo = GL_NONE;

GLuint CreateVertexArray()
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	return vao;
}

GLuint CreateBuffer()
{
	GLuint buffer;
	glGenBuffers(1, &buffer);
	return buffer;
}

void DestroyVertexArray(GLuint* vao)
{
	glDeleteVertexArrays(1, vao);
	*vao = GL_NONE;
}

void DestroyBuffer(GLuint* buffer)
{
	glDeleteBuffers(1, buffer);
	*buffer = GL_NONE;
}

void BindVertexArray(GLuint vao)
{
	assert(f_vao == GL_NONE);
	glBindVertexArray(vao);
	f_vao = vao;
}

void UnbindVertexArray(GLuint vao)
{
	assert(vao == f_vao && f_vao != GL_NONE);
	glBindVertexArray(GL_NONE);
	f_vao = GL_NONE;
}

void BindVertexBuffer(GLuint vbo)
{
	assert(f_vbo == GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	f_vbo = vbo;
}

void UnbindVertexBuffer(GLuint vbo)
{
	assert(vbo == f_vbo && f_vbo != GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	f_vbo = GL_NONE;
}

void BindIndexBuffer(GLuint ebo)
{
	assert(f_ibo == GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	f_ibo = ebo;
}

void UnbindIndexBuffer(GLuint ebo)
{
	assert(ebo == f_ibo && f_ibo != GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
	f_ibo = GL_NONE;
}

void EnableVertexAttribute(GLuint index)
{
	glEnableVertexAttribArray(index);
}

void DisableVertexAttribute(GLuint index)
{
	glDisableVertexAttribArray(index);
}

void SetVertexAttribute(GLuint index, GLint compSize, GLenum type, GLsizei stride/*, int offset*/)
{
	// No interleaved vertex data in order to simplify our interface (offset is always nullptr)
	glVertexAttribPointer(index, compSize, type, GL_FALSE, stride, nullptr);
}

void UpdateVertexBuffer(void* data, int data_size)
{
	assert(f_vbo != GL_NONE);
	glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
}

void UpdateElementBuffer(void* data, int data_size)
{
	assert(f_ibo != GL_NONE);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
}

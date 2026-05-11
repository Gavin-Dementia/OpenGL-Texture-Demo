#include "gpu/buffers/SSBO.h"

#include <iostream>

SSBO::SSBO()
{
}

SSBO::~SSBO()
{
    destroy();
}

void SSBO::create(
    size_t size,
    GLuint bindingPoint,
    GLenum usage)
{
    bufferSize = size;
    binding = bindingPoint;

    glGenBuffers(1, &id);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);

    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        size,
        nullptr,
        usage);

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        binding,
        id);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::upload(
    const void* data,
    size_t size,
    size_t offset)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);

    glBufferSubData(
        GL_SHADER_STORAGE_BUFFER,
        offset,
        size,
        data);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::bind() const
{
    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER,
        id);
}

void SSBO::bindBase() const
{
    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        binding,
        id);
}

void SSBO::destroy()
{
    if (id != 0)
    {
        glDeleteBuffers(1, &id);

        id = 0;
    }
}

GLuint SSBO::getID() const
{
    return id;
}

size_t SSBO::getSize() const
{
    return bufferSize;
}


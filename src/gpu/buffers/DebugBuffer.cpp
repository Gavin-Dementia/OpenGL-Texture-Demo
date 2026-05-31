#include "gpu/buffers/DebugBuffer.h"
#include "gpu/layout/Binding.h"

void DebugBuffer::init(GLsizeiptr maxCount)
{
    ssbo.create(maxCount);
}

void DebugBuffer::upload(const void* data, size_t size)
{
    ssbo.upload(data, size);
}

void DebugBuffer::bind() const
{    ssbo.bindBase(Binding::DebugBuffer);  }

GLuint DebugBuffer::getID() const
{    return ssbo.getID();  }


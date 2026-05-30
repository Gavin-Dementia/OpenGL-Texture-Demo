#include "gpu/buffers/VisibilityBuffer.h"
#include "gpu/layout/Binding.h"

void VisibilityBuffer::init(GLsizeiptr maxCount_)
{
    maxCount = maxCount_;
    ssbo.create(sizeof(GLuint) * maxCount);
}

void VisibilityBuffer::clear()
{
    GLuint zero = 0;
    ssbo.upload(&zero, sizeof(GLuint));
}

void VisibilityBuffer::bind() const
{    ssbo.bindBase(Binding::SSBO::VisibilityBuffer);  }

GLuint VisibilityBuffer::getID() const
{    return ssbo.getID();  }


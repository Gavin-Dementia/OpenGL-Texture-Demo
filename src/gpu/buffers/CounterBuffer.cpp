#include "gpu/buffers/CounterBuffer.h"
#include "gpu/layout/Binding.h"

void CounterBuffer::init()
{
    GLuint zero = 0;
    ssbo.create(sizeof(GLuint));
    ssbo.upload(&zero, sizeof(GLuint));
}

void CounterBuffer::reset(GLuint value)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &value);
}
void CounterBuffer::bind() const
{     ssbo.bindBase(Binding::SSBO::CounterBuffer);  }

GLuint CounterBuffer::getID() const
{    return ssbo.getID(); }


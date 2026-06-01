#include "gpu/buffers/CounterBuffer.h"
#include "gpu/layout/Binding.h"

void CounterBuffer::init()
{
    ssbo.create(sizeof(GPUCounter));

    GPUCounter zero{};
    zero.drawCount = 0;
    zero.instanceCount = 0;

    ssbo.upload(&zero, sizeof(GPUCounter));
}

void CounterBuffer::reset()
{
    GPUCounter zero{};
    zero.drawCount = 0;
    zero.instanceCount = 0;

    ssbo.upload(&zero, sizeof(GPUCounter));
}

void CounterBuffer::bind() const
{     ssbo.bindBase(Binding::CounterBuffer);  }

GLuint CounterBuffer::getID() const
{    return ssbo.getID(); }

void CounterBuffer::uploadInstanceCount(GLuint count)
{
    GPUCounter data{};

    data.drawCount = 0;
    data.instanceCount = count;

    ssbo.upload(&data, sizeof(GPUCounter));
}

GLuint CounterBuffer::read() const
{
    GPUCounter data{};

    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER,
        ssbo.getID()
    );

    glGetBufferSubData(
        GL_SHADER_STORAGE_BUFFER,
        0,
        sizeof(GPUCounter),
        &data
    );

    return data.drawCount;
}

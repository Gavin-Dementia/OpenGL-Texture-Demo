#include "gpu/buffers/InstanceBuffer.h"
#include "gpu/layout/Binding.h"

void InstanceBuffer::init(GLsizeiptr maxInstances)
{
    maxCount = maxInstances;
    count = 0;

    ssbo.create(sizeof(Instance) * maxInstances);
}

void InstanceBuffer::upload(const Instance* data, size_t count_)
{
    count = static_cast<GLuint>(count_);
    ssbo.upload(data, sizeof(Instance) * count_);
}

void InstanceBuffer::bind() const
{    ssbo.bindBase(Binding::SSBO::InstanceBuffer); }

GLuint InstanceBuffer::getID() const
{    return ssbo.getID();  }


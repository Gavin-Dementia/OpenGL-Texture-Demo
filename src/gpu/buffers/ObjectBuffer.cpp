#include "gpu/buffers/ObjectBuffer.h"
#include "gpu/layout/GPUObjectData.h"
#include "gpu/layout/Binding.h"

void ObjectBuffer::init()
{
    ssbo.create(
        sizeof(GPUObjectData),
        GL_DYNAMIC_DRAW);
}

void ObjectBuffer::upload(
    const std::vector<GPUObjectData>& objects)
{
    if (objects.empty())
        return;

    ssbo.upload(
        objects.data(),
        sizeof(GPUObjectData) * objects.size());
}

void ObjectBuffer::bind() const
{    ssbo.bindBase(Binding::ObjectBuffer);  }

size_t ObjectBuffer::size() const
{    return ssbo.getSize() / sizeof(GPUObjectData);  }


#include "gpu/buffers/TransformBuffer.h"
#include "gpu/layout/Binding.h"
#include "gpu/layout/GPUTransformData.h"

void TransformBuffer::init()
{
    ssbo.create(
        sizeof(GPUTransformData),
        GL_DYNAMIC_DRAW);
}

void TransformBuffer::upload(
    const std::vector<GPUTransformData>& transforms)
{
    if (transforms.empty())
        return;

    ssbo.upload(
        transforms.data(),
        sizeof(GPUTransformData) * transforms.size());
}

void TransformBuffer::bind() const
{    ssbo.bindBase(Binding::SSBO::TransformBuffer);  }

GLuint TransformBuffer::size() const
{    return ssbo.getSize() / sizeof(GPUTransformData);  }


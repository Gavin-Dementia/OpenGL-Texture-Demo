#include"gpu/buffers/MaterialBuffer.h"
#include "gpu/layout/GPUMaterialData.h"
#include "gpu/layout/Binding.h"

void MaterialBuffer::init()
{
    ssbo.create(
        sizeof(GPUMaterialData),
        GL_DYNAMIC_DRAW);
}

void MaterialBuffer::upload(
    const std::vector<GPUMaterialData>& materials)
{
    if (materials.empty())
        return;

    ssbo.upload(
        materials.data(),
        sizeof(GPUMaterialData) * materials.size());
}

void MaterialBuffer::bind() const
{    ssbo.bindBase(Binding::MaterialBuffer);  }

size_t MaterialBuffer::size() const
{    return ssbo.getSize() / sizeof(GPUMaterialData);  }


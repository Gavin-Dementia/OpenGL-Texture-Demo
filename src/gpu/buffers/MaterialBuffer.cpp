#include"gpu/buffers/MaterialBuffer.h"
#include "gpu/layout/GPUMaterialData.h"

void MaterialBuffer::upload(const std::vector<GPUMaterialData>& materials)
{
    ssbo.upload(materials.data(),
                materials.size() * sizeof(GPUMaterialData));
}


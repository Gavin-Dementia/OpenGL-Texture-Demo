#ifndef MATERIALBUFFER_H
#define MATERIALBUFFER_H

#include <glad/glad.h>
#include <vector>

#include "gpu/buffers/SSBO.h"

class GPUMaterialData;

class MaterialBuffer
{
public:

    void upload(
        const std::vector<GPUMaterialData>& materials);

private:

    SSBO ssbo;
};
#endif // MATERIALBUFFER_H


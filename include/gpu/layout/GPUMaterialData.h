#ifndef GPU_MATERIAL_DATA_H
#define GPU_MATERIAL_DATA_H
#include <glm/glm.hpp>
#include <cstdint>

struct GPUMaterialData
{
    glm::vec4 albedo;
    glm::vec4 emissive;

    uint32_t diffuseTex;
    uint32_t specularTex;

    uint32_t albedoTexture;
    uint32_t normalTexture;
    uint32_t metallicRoughnessTexture;

    float shininess;
    uint32_t flags;
};

#endif // GPU_MATERIAL_DATA_H


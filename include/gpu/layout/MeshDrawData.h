#ifndef GPU_MESH_DRAW_DATA_H
#define GPU_MESH_DRAW_DATA_H
#include <glm/glm.hpp>

struct MeshDrawData
{
    uint32_t vertexOffset;
    uint32_t indexOffset;

    uint32_t vertexCount;
    uint32_t indexCount;

    uint32_t boundingSphereOffset;
    // xyz = center (model space)  ,   w   = radius
    uint32_t materialOffset;

    uint32_t pad0;
    uint32_t pad1;
};

#endif // GPU_MESH_DRAW_DATA_H


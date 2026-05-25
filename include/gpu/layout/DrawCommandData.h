#ifndef DRAWCOMMANDDATA_H
#define DRAWCOMMANDDATA_H

#include <cstdint>
#include <glm/glm.hpp>

struct DrawCommandData
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

    // OpenGL indirect draw fields
    // uint32_t indexCount;      // number of indices
    // uint32_t instanceCount;   // number of instances
    // uint32_t firstIndex;      // offset in index buffer
    // int32_t  baseVertex;      // base vertex offset
    // uint32_t baseInstance;    // first instance ID

    // uint32_t objectID;        // link to ObjectBuffer
    // uint32_t materialID;      // link to MaterialBuffer

    // uint32_t transformID;     // link to TransformBuffer

    // uint32_t visibilityMask;  // used by compute culling (0/1 or bitmask)

    // // optional debug / LOD
    // uint32_t lodLevel;
};

#endif // DRAWCOMMANDDATA_H


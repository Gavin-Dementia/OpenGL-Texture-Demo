#ifndef GPU_OBJECT_DATA_H
#define GPU_OBJECT_DATA_H
#include <cstdint>

struct GPUObjectData
{
    uint32_t transformID;
    uint32_t meshID;
    uint32_t materialID;

    uint32_t visibilityID;
};

#endif // GPU_INSTANCE_H


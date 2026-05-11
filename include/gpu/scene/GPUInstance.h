#ifndef GPU_INSTANCE_H
#define GPU_INSTANCE_H
#include <cstdint>

struct GPUInstance
{
    uint32_t transformID;
    uint32_t meshID;
    uint32_t materialID;

    uint32_t visibilityID;
};

#endif // GPU_INSTANCE_H



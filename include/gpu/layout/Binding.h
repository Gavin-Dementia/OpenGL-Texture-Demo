#ifndef BINDING_REGISTRY_H
#define BINDING_REGISTRY_H

#include <glad/glad.h>

namespace Binding
{
    enum SSBO
    {
        ObjectBuffer     = 0,
        TransformBuffer  = 1,
        MeshBoundsBuffer = 2,
        InstanceBuffer   = 3,
        DrawBuffer       = 4,
        MaterialBuffer   = 5,
        CounterBuffer    = 6,
        DebugBuffer      = 7,
        VisibilityBuffer = 8,
        DirLightBuffer   = 9,
        PointLightBuffer = 10,
        SpotLightBuffer  = 11
    };
}

#endif// BINDING_REGISTRY_H


#ifndef BINDING_REGISTRY_H
#define BINDING_REGISTRY_H

#include <glad/glad.h>

namespace Binding
{
    static constexpr uint32_t ObjectBuffer     = 0;
    static constexpr uint32_t TransformBuffer  = 1;
    static constexpr uint32_t MeshBoundsBuffer = 2;
    static constexpr uint32_t InstanceBuffer   = 3;
    static constexpr uint32_t DrawBuffer       = 4;
    static constexpr uint32_t MaterialBuffer   = 5;
    static constexpr uint32_t CounterBuffer    = 6;
    static constexpr uint32_t DebugBuffer      = 7;
    static constexpr uint32_t VisibilityBuffer = 8;
    static constexpr uint32_t DirLightBuffer   = 9;
    static constexpr uint32_t PointLightBuffer = 10;
    static constexpr uint32_t SpotLightBuffer  = 11;
}

#endif// BINDING_REGISTRY_H


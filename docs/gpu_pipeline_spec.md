# GPU Pipeline Specification (OpenGL 4.3 Experimental → Vulkan Migration Base)

This document defines the GPU-driven rendering pipeline used in the `gpu-pipeline-base` branch.  
It serves as both:

- OpenGL 4.3 implementation reference
- Migration blueprint toward Vulkan-style rendering architecture

---

# 1. Overview

The pipeline is designed to shift rendering responsibility from CPU to GPU using:

- SSBO-based scene representation
- Compute shader visibility culling
- Indirect draw command generation
- GPU-driven execution model

---

## High-Level Flow

```cpp
CPU Scene Build
↓
GPU Upload (SSBO)
↓
Compute Shader (Culling)
↓
Indirect Command Generation
↓
Memory Barrier Synchronization
↓
glMultiDrawElementsIndirect
↓
Fragment Shading
```

---

# 2. Core Design Goals

## 2.1 GPU-driven execution

Minimize CPU involvement in per-frame rendering decisions:

- CPU: scene update + upload only
- GPU: visibility + draw submission

---

## 2.2 Data-oriented rendering

All renderable data is flattened into GPU buffers:

- TransformBuffer (SSBO)
- InstanceBuffer (SSBO)
- MeshBuffer (SSBO)
- VisibilityBuffer (SSBO)
- IndirectCommandBuffer

---

## 2.3 Decoupled pipeline stages

Each stage is independent:

- Scene construction
- Culling
- Command generation
- Draw execution

---

# 3. GPU Data Model

## 3.1 Instance

Represents a renderable entity referencing GPU resources
```cpp
struct Instance
{
    uint transformID;
    uint meshID;
    uint materialID;
    uint visibilityID;
};
```

---

## 3.2 Transform Buffer

Stores world transforms
```cpp
layout(std430, binding = 1) buffer TransformBuffer
{
    mat4 transforms[];
};
```

---

## 3.3 Visibility Buffer
```cpp
layout(std430, binding = 8) buffer VisibilityBuffer
{
    uint instanceIndices[];// or vidibilityBuffer[]
};
```
Stores visible instance indices after culling.

---

## 3.4Indirect Draw Command
```cpp
struct DrawElementsIndirectCommand
{
    uint count;
    uint instanceCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};
```

---

# 4. Compute Culling Pipeline

## 4.1 Responsibilities

Compute shader performs:

- Frustum culling
- Visibility determination
- Writing visible instance indices
- Incrementing draw counter

---

## 4.2 Execution Model
```cpp
Dispatch size = ceil(instanceCount / 64)

Each invocation processes a batch of instances.
```

---

## 4.3 Output
```cpp
VisibilityBuffer (visible instance IDs)
CounterBuffer (drawCount)
```

---

## 4.4 Synchronization

Required barrier:
```cpp
glMemoryBarrier(
    GL_SHADER_STORAGE_BARRIER_BIT |
    GL_COMMAND_BARRIER_BIT
);
```

---

# 5. Indirect Draw Pipeline

## 5.1 Binding Requirements

Before draw:

- VAO bound
- EBO bound
- SSBOs bound
- Indirect buffer bound

---

## 5.2 Execution
```cpp
glMultiDrawElementsIndirect(
    GL_TRIANGLES,
    GL_UNSIGNED_INT,
    nullptr,
    drawCount,
    0
);
```

---
## 5.3 Key Assumption

drawCount must match visibility results written by compute stage.

---

# 6. Pipeline Constraints (OpenGL 4.3)

This architecture is constrained by OpenGL 4.3 limitations:

## 6.1 Global state machine
- No pipeline object encapsulation
- State leakage between passes is possible

---

## 6.2 Indirect execution ambiguity
- Execution order is not strictly guaranteed
- Buffer visibility depends on explicit barriers

---

## 6.3 No bindless resource model (core)
- Resource lookup depends on SSBO indexing
- Indirection cost is CPU/GPU hybrid

---

## 6.4 Synchronization fragility

Common failure modes:

- compute writes not visible to draw
- indirect buffer not updated in time
- counter mismatch between GPU/CPU readback

---

# 7. Observed Failure Cases
## 7.1 drawCount mismatch

Cause:

- race between compute output and CPU readback

---

## 7.2 black frame despite valid SSBO

Cause:

- VAO/EBO mismatch or missing binding state

---

## 7.3 compute correct but no draw output

Cause:

- missing memory barrier or indirect buffer misbinding

---

## 7.4 intermittent correctness

Cause:

- undefined ordering in OpenGL execution model

---

# 8. Mapping to Vulkan Architecture

This pipeline maps cleanly to Vulkan concepts:

| OpenGL 4.3          | Vulkan equivalent          |
| ------------------- | -------------------------- |
| SSBO                | Storage Buffer             |
| glDispatchCompute   | vkCmdDispatch              |
| glMemoryBarrier     | pipeline barrier           |
| glMultiDrawIndirect | vkCmdDrawIndirect          |
| global state        | pipeline + descriptor sets |

---

# 9. Design Conclusion

- GPU-driven rendering is structurally valid
- OpenGL 4.3 cannot guarantee deterministic execution
- Pipeline correctness depends on implicit driver behavior

---

# 10. Future Migration Plan (Vulkan)

Planned evolution:

## Stage 1

- Replace SSBO bindings with descriptor sets
- Explicit buffer ownership model

## Stage 2

- Replace indirect draw with Vulkan command buffers

## Stage 3

- Introduce render graph system

## Stage 4

- Fully GPU-driven command generation pipeline

---

# 11. Summary

This pipeline represents a functional GPU-driven rendering prototype under OpenGL 4.3 constraints, and serves as a direct architectural stepping stone toward Vulkan-based rendering systems.


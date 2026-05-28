# GPU Memory Layout Specification

## Overview

This document defines the strict memory layout rules for all GPU buffers used in the renderer.

All CPU and GPU structures MUST match exactly under std430 layout rules.

---

## Layout Rules (std430)

- Scalars (uint, int, float): 4 bytes
- vec3: treated as vec4 alignment (16 bytes)
- vec4: 16 bytes
- mat4: 4 vec4 columns (64 bytes total)
- Array elements aligned to base type alignment

---

## Buffer Binding Map

| Binding | Buffer Name         |
|-------- |---------------------|
| 0       | ObjectBuffer        |
| 1       | TransformBuffer     |
| 2       | MeshBoundsBuffer    |
| 3       | InstanceBuffer      |
| 4       | DrawBuffer          |
| 5       | MaterialBuffer      |
| 6       | CounterBuffer       |
| 7       | DebugBuffer         |
| 8       | VisibilityBuffer    |
| 9       | DirLightBuffer      |
| 10      | PointLightBuffer    |
| 11      | SpotLightBuffer     |


---

## Core Structures

### Instance

```cpp
struct Instance
{
    uint transformID;
    uint meshID;
    uint materialID;
    uint visibilityID;
};
```

### IndirectCommandBuffer
```cpp
struct IndirectCommandBuffer
{
    uint count;
    uint instanceCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};
```
### Bounding Sphere

Stored as vec4:
```cpp
struct BoundingSphere
{
    vec3 center;
    float radius;
};
xyz = center (world or object space depending on stage)
w = radius
```
## Buffer Definitions
### ObjectBuffer

Contains all scene instances.

Used as input to visibility compute shader.

TransformBuffer

Array of mat4 transforms.

Index mapped by Instance.transformID.
### MeshBoundsBuffer

Stores bounding spheres per mesh:

Used for frustum culling
Indexed by Instance.visibilityID
VisibilityBuffer

Compact list of visible instances:

Written using atomic counter
Used for indirect draw generation
DrawBuffer

Indirect command buffer:

One entry per visible instance
Used by glDrawElementsIndirect
CounterBuffer

Contains:

uint drawCount;

Must be reset to 0 every frame before compute dispatch.

### DebugBuffer

Per-instance debug color output:

Used for visualization of culling results


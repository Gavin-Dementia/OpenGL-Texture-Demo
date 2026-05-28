# GPU Pipeline Design

## Overview

This project implements a GPU-driven rendering pipeline based on compute-shader visibility culling and indirect draw command generation.

The pipeline is designed to minimize CPU-GPU synchronization and fully utilize GPU parallelism for per-instance visibility evaluation.

---

## Pipeline Stages

### 1. CPU Scene Upload

CPU side responsibilities:

- Build Scene (Mesh / Material / Transform / Instance)
- Upload all required SSBO buffers:
  - ObjectBuffer
  - TransformBuffer
  - MeshBoundsBuffer
  - MaterialBuffer (if used)

No per-frame CPU culling is performed.

---

### 2. Visibility Pass (Compute Shader)

Compute shader responsibilities:

- Perform frustum culling per instance
- Evaluate bounding sphere against view-projection matrix
- Write visible instances into compact buffer
- Atomically increment visible count

Output:

- VisibilityBuffer (compact instance list)
- CounterBuffer.drawCount

---

### 3. Indirect Command Generation

Each visible instance generates:

- IndirectCommandBuffer entry
- baseInstance = compacted index
- drawCount = mesh index count (static or per-mesh)

This stage can be:
- merged into visibility compute shader (current design)
- or separated into a second compute pass (future optimization)

---

### 4. Draw Pass (Indirect Rendering)

GPU executes:

- glMultiDrawIndirect or glDrawElementsIndirect
- No CPU intervention required per object

Input:

- DrawBuffer (IndirectCommandBuffer)
- VisibilityBuffer (optional debugging)

---

### 5. Debug Pass (Optional)

Used for:

- Visualizing visibility results
- Coloring visible / culled objects
- Debugging culling correctness

---

## Execution Order per Frame

1. Reset CounterBuffer.drawCount = 0
2. Dispatch Visibility Compute Shader
3. Memory barrier (SSBO + atomic)
4. Issue indirect draw call
5. Optional debug pass

---

## Design Constraints

- No per-object CPU draw submission
- All per-frame visibility handled on GPU
- Buffers must remain tightly packed (std430 layout)
- Atomic operations are the only synchronization primitive in compute stage

---

## Future Extensions

- Hierarchical Z culling (Hi-Z)
- Cluster-based culling (meshlets)
- Material batching in GPU
- Multi-draw indirect per material group



---

# 📄 `docs/architecture.md`

```md
# Renderer Architecture Overview

## System Goal

This renderer is designed as a GPU-driven architecture where:

- CPU only manages scene state and uploads
- GPU performs visibility, culling, and draw submission
- Rendering is fully indirect and parallelized

---

## High-Level Architecture
Scene (CPU)
↓
SceneUploader
↓
GPU Buffers (SSBO)
↓
VisibilityPipeline (Compute Shader)
↓
VisibilityBuffer + DrawBuffer
↓
DrawPipeline (Indirect Rendering)
↓
Frame Output


---

## Module Responsibilities

### RendererGPU

Central orchestrator:

- Initializes GPU resources
- Manages frame lifecycle
- Dispatches compute + draw passes

---

### SceneUploader

Responsibilities:

- Converts CPU Scene → GPU buffers
- Uploads:
  - Instances
  - Transforms
  - Mesh bounds
- Ensures stable indexing

---

### VisibilityPipeline

Compute shader pipeline:

- Performs frustum culling
- Generates compact visible list
- Writes indirect draw data

Key output:
- VisibilityBuffer
- CounterBuffer

---

### DrawPipeline

Responsible for:

- Indirect draw submission
- Binding shaders/materials
- Executing glDrawElementsIndirect

---

### Buffers Layer

Encapsulates GPU memory objects:

- SSBO abstraction
- Binding management
- Allocation / resize logic

---

### FrameContext

Holds per-frame state:

- Camera matrices
- ViewProj matrix
- Frame counters
- Temporary GPU states

---

## Frame Lifecycle

### 1. BeginFrame

- Reset CounterBuffer.drawCount = 0
- Update camera matrices
- Upload dynamic transforms (if needed)

---

### 2. Upload Phase

- SceneUploader syncs CPU → GPU
- Only dirty data is updated

---

### 3. Compute Phase

- Dispatch Visibility Compute Shader
- Perform culling
- Write visible instances

---

### 4. Barrier Sync

- glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT)

Ensures compute writes are visible to draw stage.

---

### 5. Draw Phase

- Execute indirect draw calls
- No CPU per-object submission

---

### 6. EndFrame

- Debug output (optional)
- Profiling / counters reset (if needed)

---

## Design Philosophy

- GPU is the primary compute authority
- CPU is a scene descriptor, not a renderer
- All per-object decisions must be parallelized
- Avoid CPU-GPU ping-pong

---

## Future Extensions

- Hierarchical culling (BVH / cluster)
- Meshlet-based rendering
- GPU material sorting
- Multi-pass lighting pipeline
- Compute-based LOD selection


#  Update Log — Rendering Pipeline Refactor (RenderPass + Shadow Mapping)

**Date:** 2026-04-25
---

## Summary

This update extends the existing RenderPass-based architecture with a **first working GPU-driven culling pipeline prototype**.
The GPU culling pipeline currently operates alongside the existing RenderPass pipeline and does not yet fully replace CPU-side render scheduling.

Key additions:

- Initial **Compute Shader integration (`cull_and_build_indirect.comp`)**
- Introduction of **SSBO-based render queue**
- First step toward **GPU-driven indirect rendering pipeline**
- CPU → GPU transition validation (hybrid fallback still present)

The renderer is now transitioning from a **CPU-instanced renderer → hybrid GPU-driven renderer**.

---

## New Additions

## 1. Compute Shader Pipeline (GPU Culling Prototype)

A compute shader pipeline has been introduced:

**Shader:** `cull_and_build_indirect.comp`

### Responsibilities:

- Reads per-object data from **SSBO render queue**
- Performs basic **frustum / clip-space culling**
- Writes visible objects into:(partially implemented)
  - Indirect draw command buffer(experimental)
  - Instance / transform buffer (planned extension)

### Dispatch Flow:
CPU Render Queue
↓
SSBO Upload
↓
Compute Shader (prepare for Culling)
↓
Indirect Draw Buffer Generation
↓
Rendering Stage

---

## 2. SSBO-Based Render Queue (GPU Backbone)

### Buffer:

```cpp
struct GpuRenderItem {
    uint meshID;
    uint materialID;
    uint emissive;
    uint pad;
    mat4 model;
    vec4 emissiveColor;
};
```
### Binding:
The SSBO is currently reallocated or updated per frame in a dynamic fashion (GL_DYNAMIC_DRAW), with future plans to move toward persistent mapped buffers for performance.

Purpose:
 - Replaces CPU-only iteration for rendering preparation
 - Provides GPU-readable scene representation
 - Serves as input for compute culling stage
---
## 3. drawObjectsGPU (Hybrid Transition Path)

A new hybrid pipeline function is introduced:

Flow:
 - Build CPU render queue (RenderItem)
 - Convert to GPU format (GpuRenderItem)
 - Upload to SSBO (ssboRenderQueue)
 - Dispatch compute shader (culling stage)
 - Fallback to CPU rendering (debug / validation)

Purpose:
 - Ensure correctness before full GPU migration
 - Validate SSBO + compute pipeline stability
 - Provide safe fallback during development
---
## 4. Renderer Initialization Updates
SSBO Setup:
```cpp
glGenBuffers(1, &ssboRenderQueue);
glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboRenderQueue);
glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, ssboRenderQueue);
```
---
Result:
 - Engine now supports GPU-readable scene data
 - Compute pipeline fully bindable in rendering stage
## 5. Compute Shader Integration Fix
Original Shader Issue, class only supported:

 - Vertex + Fragment shaders

Fix:
 - Added Compute Shader constructor support

Now supported:
```cpp
GL_VERTEX_SHADER
GL_FRAGMENT_SHADER
GL_COMPUTE_SHADER
```
Result:

 - Compute pipeline is now fully integrated into engine architecture.

### Current Architecture State:

The renderer is transitioning toward a GPU-driven pipeline, with compute-based culling currently implemented as a prototype stage. Indirect draw generation is planned but not yet fully integrated.

---

**Date:** 2026-04-22

---

##  Summary

This update introduces a **modular RenderPass-based rendering pipeline**, along with:

-   Directional light **shadow mapping (depth pass)**
-   **Render queue and instanced batching**
-   Clean separation between **Depth / Forward / Light passes**

The renderer is now structured closer to a **modern real-time rendering pipeline**, making it ready for future enhancements such as:

-   GPU-driven rendering
-   Deferred rendering
-   Compute-based culling

---

## Architectural Changes

### 1. RenderPass Abstraction (Core Upgrade)

**New Modules:**
-   `include/RenderPasses.h`
-   `src/core/RenderPasses.cpp`

### Pipeline Flow

The rendering process now follows a distinct, sequential flow:

`DepthPass` → `ForwardPass` → `LightPass`

### Benefits

-   **Decoupled Render Logic:** Separates complex rendering logic from the main `Renderer` class, promoting cleaner code and easier maintenance.
-   **Independent Scalability:** Each pass can be independently extended and optimized, facilitating the integration of advanced rendering techniques (e.g., GPU culling, deferred shading).
-   **Modern Pipeline Alignment:** Aligns the renderer's structure with common practices in modern game engines like Unreal Engine and Unity.

---

## 2. Shadow Mapping (Directional Light)

### Implementation

**Depth Pass**

-   Utilizes an **orthographic projection** for directional lights to generate a shadow map.
-   The entire scene is rendered into a **depth texture** (the shadow map).

```cpp
glm::mat4 lightProjection = glm::ortho(...);
glm::mat4 lightView = glm::lookAt(...);
glm::mat4 lightSpaceMatrix = lightProjection * lightView;
```

-   **Framebuffer Configuration:**
    -   Dedicated depth-only FBO (Frame Buffer Object).
    -   Resolution: 1024 × 1024.

-   **Shader Modifications:**
    -   `depth.vert`: Outputs the vertex position in light space.
    -   `basic.vert`: Declares `out vec4 FragPosLightSpace;` to pass light-space coordinates to the fragment shader.
    -   `basic.frag`:
        -   Introduces `uniform sampler2D shadowMap;` for sampling the depth texture.
        -   Performs **depth comparison** for shadow testing.
        -   Includes a **bias mechanism** to prevent shadow acne artifacts.

---

## 3. Render Queue + Instancing

### New Concept: `RenderItem`

A new struct `RenderItem` now encapsulates per-instance rendering data:

```cpp
struct RenderItem {
    Mesh* mesh;
    Material* material;
    glm::mat4 model;
    // ... other instance-specific data like isEmissive, emissiveColor ...
};
```

### Pipeline

1.  **Queue Construction:** A render queue (`std::vector<RenderItem>`) is built, with one `RenderItem` for each model instance in the scene.
2.  **Sorting:** The queue is sorted to optimize rendering performance, primarily by:
    -   `material` pointer (primary key)
    -   `mesh` pointer (secondary key)
3.  **Batching:** Contiguous items with the same `(material, mesh)` combination are batched together and rendered using **instanced drawing**.

### Benefits

-   **Reduced Draw Calls:** Grouping similar objects significantly minimizes the number of API calls to the GPU.
-   **Minimized State Changes:** Sorting by material and mesh reduces costly GPU state changes (e.g., texture binds, shader switches).
-   **Foundation for GPU-Driven Rendering:** The `RenderItem` structure and sorting approach lay the groundwork for future GPU-driven rendering techniques.

---

## 4. RenderPass Implementation Details

### 🔹 `DepthPass`

-   Responsible for rendering the scene from the light's perspective to generate shadow maps.
-   Internally calls `renderer.executeDrawObjects(scene, depthShader);` to render all potential shadow casters.
-   Returns the `lightSpaceMatrix` for use in subsequent passes.

### 🔹 `ForwardPass`

-   Executes the primary scene rendering.
-   Calls `renderer.uploadCameraPublic(...)` and `renderer.uploadLightsPublic(...)` to set up camera and light uniform buffer objects (UBOs).
-   Performs normal forward rendering, utilizing the shadow map generated by the `DepthPass` for accurate lighting.

### 🔹 `LightPass`

-   Dedicated to rendering debug visualizations for light sources.
-   For each light visualizer object (`obj`), it calls `obj.mesh->draw();` to render a simplified representation (e.g., small cubes).

---

## Current Limitations

### Shadow

-   **Hard Shadows Only:** Currently lacks soft shadow techniques (e.g., PCF - Percentage-Closer Filtering).
-   **Single Directional Light:** Only supports shadow mapping for one directional light at a time.
-   **Fixed Orthographic Size:** The orthographic projection size for the shadow map is hardcoded and not dynamically adjusted.

### Material System

-   **Incomplete Binding Pipeline:** While a `Material` system is introduced, the full material binding pipeline (e.g., more complex textures, material properties) is not yet fully integrated across all shaders.
-   **CPU-Side Batching:** The current batching mechanism relies heavily on CPU sorting and processing, which can be a bottleneck for very large scenes.

### Pipeline

-   **GPU Culling Not Implemented:** There is no GPU-based frustum culling or occlusion culling in place.
-   **Indirect Draw Not Implemented:** Instance rendering is still driven by direct draw calls rather than GPU-generated indirect draw commands.

```
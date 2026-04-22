**Bugs Log**

# 15/04/2026

- **Instance Attributes Missing**: instance matrices for instanced draws were never initialized, causing per-instance transforms to be incorrect or effectively zeroed.
  - **Symptom**: black screen / instanced objects invisible or collapsed to origin.
  - **Cause**: per-instance vertex attributes (locations 3..6) and `instanceVBO` were not set up before uploading instance data.
  - **Fix**: call `setupInstanceBuffer()` after creating the mesh VAO (done in `src/objects/Cube.cpp`) or add a defensive guard in `Mesh::setInstances()` to call `setupInstanceBuffer()` when needed.
  - **Files**: [src/objects/Cube.cpp](src/objects/Cube.cpp), [src/objects/Mesh.cpp](src/objects/Mesh.cpp)

- **Uniform Name Mismatch (light model)**: the light object shader expected a different uniform name than the renderer provided.
  - **Symptom**: light debug markers not visible or incorrectly transformed.
  - **Cause**: `shaders/light.vert` uses `modelLight` while `Renderer` set `modelMat` (mismatch).
  - **Fix**: set the matching uniform name in the renderer (`lightShader.setMat4("modelLight", ...)`) — applied in `src/core/Renderer.cpp`.
  - **Files**: [src/core/Renderer.cpp](src/core/Renderer.cpp), [shaders/light.vert](shaders/light.vert)

- **Potential Texture/Shader Robustness Issues (suggestions)**:
  - **Symptom**: occasional "Failed to load texture" messages or shader load errors if paths are wrong.
  - **Suggestions**: ensure texture file paths are correct; set default texture parameters (`glTexParameteri`) in `Texture` to avoid sampling issues; check `Shader` constructor logs for file open errors and print full paths on failure.
  - **Files**: [src/graphics/Texture.cpp](src/graphics/Texture.cpp), [src/graphics/Shader.cpp](src/graphics/Shader.cpp)

**How to verify fixes**

1. Build the project and run the executable.
2. Expected result: nine textured cubes visible and two white light-cube markers.

**Recommended follow-ups**

- Add a defensive guard in `Mesh::setInstances()` to call `setupInstanceBuffer()` if `instanceVBO == 0`.
- Add more verbose logging in `Shader` constructor when shader files fail to open or compile.
- Add unit/visual smoke test or a small startup scene that validates instanced rendering and light markers automatically.

**Status**: fixes applied to `src/objects/Cube.cpp` and `src/core/Renderer.cpp`.

# 19/04/2026

- **Directional / Point / Spot lights migrated to UBOs (std140)**
  - **Symptom**: initially screen was black with no visible lighting.
  - **Cause**: Camera/DirLight/PointLight/SpotLight data were uploaded as individual uniforms or not uploaded into GPU-friendly std140 layout; `dirLightGPU` and other GPU caches were not filled prior to UBO upload; `Renderer::init()` was not called before rendering.
  - **Fixes applied**:
    - Added `Camera` UBO (binding 0) and `DirLight` UBO (binding 1) and moved camera/view/proj/viewPos into the camera UBO.
    - Replaced point lights and spot light uniforms with `PointLightsBlock` (binding 2) and `SpotLightBlock` (binding 3) using std140 GPU structs (`PointLightGPU`, `SpotLightGPU`).
    - Populated GPU-side cache structs (`dirLightGPU`, `pointLightsGPU`, `spotLightGPU`) from CPU `LightManager` and uploaded them with `glBufferSubData`.
    - Ensure `renderer.init()` is called in `main` to create and bind UBO ranges before rendering.
    - Added defensive shader block binding in `main.cpp` (using `glUniformBlockBinding`) to map blocks to binding points at runtime.
    - Added lightweight runtime debug printouts and UBO readback to verify uploaded contents.
  - **Files**: `include/Scene.h`, `include/Renderer.h`, `src/core/Renderer.cpp`, `src/main.cpp`, `shaders/basic.frag`, `shaders/light.vert`.
  - **Status**: verified locally — executable launches and textures load; UBO readback shows non-zero light data.

  - **Light visuals not following point lights (fixed)**
    - **Symptom**: white emissive debug cubes (lightVisuals) remained static while point light positions animated, causing visual mismatch between light source and its marker.
    - **Cause**: `Scene::update()` updated `lights.pointLights` but did not propagate positions to `scene.lightVisuals` (the debug `Object` instances used for rendering light markers).
    - **Fix**: synchronize visual objects with light positions inside `Scene::update()`; also changed point-light animation to use an accumulated time value rather than per-frame `deltaTime` so motion is smooth and not near-zero. (See `src/scene/Scene.cpp`)
    - **Files**: [src/scene/Scene.cpp](src/scene/Scene.cpp)

  **Status**: patched and verified — visual markers now move with their corresponding point lights.

**Recommended follow-ups**:
- Add guard in `Mesh::setInstances()` to ensure instance VBO is created before use (see earlier note).
- Improve shader compile error logging in `Shader` to print full infoLog and shader file path on failure.
- Add an automated startup smoke test that asserts non-black framebuffer after a first frame render (helps catch regressions).



# 22/04/2026
## Technical Retrospective: GPU-Driven Pipeline Integration Failure

---

### Executive Summary

An attempt to implement a high-performance GPU-driven rendering pipeline (comprising compute-based frustum culling and multi-draw indirect commands) resulted in critical system instability. Deployment was halted due to fundamental architectural mismatches and API-level incompatibilities.

---

### Root Cause Analysis (RCA)

#### 1. GLSL Versioning and Extension Constraints
- **Incident**: Compute shader compilation failure.
- **Root Cause**: The implementation targeted GLSL features (Version 4.20+) not explicitly supported by the current OpenGL context. Furthermore, `GL_ARB_compute_shader` was not correctly requested in the initialization phase.

#### 2. Type Specification Violations
- **Incident**: Precision errors reported during shader linking.
- **Root Cause**: Implicit usage of `double` precision types, which are unsupported in the standard Core Profile without specific hardware extensions.

#### 3. Shader Abstraction Limitations
- **Incident**: Inability to dispatch compute kernels.
- **Root Cause**: The existing `Shader` class was architected exclusively for Vertex and Fragment stages. It lacked the necessary logic for compute program creation, work-group dispatch, and memory barrier synchronization.

#### 4. SSBO Memory Alignment and Binding Discrepancies
- **Incident**: Illegal memory access or zero-data rendering.
- **Root Cause**: Inconsistent binding points between the host (C++) and device (GLSL). Critical handles such as `RENDER_QUEUE_SSBO_BINDING` were improperly scoped, leading to unaligned data mapping.

#### 5. Command Buffer Layout Mismatch
- **Incident**: Failed execution of Indirect Draw calls.
- **Root Cause**: Discrepancies between the CPU `DrawIndirectCommand` struct and the GPU-side layout (std430 alignment rules), resulting in corrupted command arguments.

#### 6. Core Architectural Instability
- **Incident**: Cascading compilation and runtime errors.
- **Root Cause**: Attempted to integrate advanced GPU-driven features onto a non-stabilized renderer base, creating circular dependencies between the Scene Graph and the GPU Pipeline.

---

### Strategic Takeaways

1.  **Architecture Prioritization**: Advanced GPU features require a robust, decoupled modular architecture. Logic cannot be "injected" into a monolithic renderer without prior refactoring.
2.  **API Strictness**: Absolute consistency across OpenGL versions, GLSL declarations, and GLAD capabilities is mandatory for compute operations.
3.  **Memory Layout Precision**: SSBO and Indirect Buffer implementations require 100% bit-accurate alignment between Host and Device structures.

---

### Remediation Action Plan

To mitigate further instability, the project has been rolled back to the last known stable state.
**Current Status**: Reverting to `main` branch; the `gpu-experiment-broken` branch has been archived for reference.

---

### Engineering Roadmap (Phase 2 Refactor)

The GPU pipeline will be reintroduced incrementally following these milestones:

1.  **Finalize RenderPass Abstraction**: Stabilize the multi-pass system (Depth, Forward, Post). ✅
2.  **Refactor Render Queue**: Implement a clean, sortable CPU-side queue as a foundation.
3.  **SSBO Integration**: Develop a dedicated interface for GPU buffer management and alignment verification.
4.  **Compute Integration**: Extend the `Shader` class to support compute-specific lifecycle management.
5.  **Indirect Execution**: Implement verified Indirect Draw commands with proper memory barriers.

---

### Final Assessment
While the implementation failed to meet the deployment criteria, the diagnostic process successfully identified systemic architectural weaknesses that must be addressed to support a production-grade GPU-driven renderer.

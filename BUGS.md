**Bug Log**

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

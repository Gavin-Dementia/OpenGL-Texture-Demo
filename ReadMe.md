# OpenGL Texture Rendering Demo

## Features

- OpenGL 3.3 Core Profile
- Texture mapping (multi-texture blending)
- Shader abstraction (C++ class wrapper)
- stb_image image loading
- Basic camera system (FPS-style)
- Phong lighting model (Directional + Point + Spot lights)
- Scene / Renderer separation (mini engine architecture v0.1)

---

## Demo

Render a textured cube scene with:
- Multiple point lights
- Directional light
- Camera-controlled first-person view
- Debug light visualization cubes

---

## Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```
---

## Dependencies

```bash
GLFW
GLAD
stb_image
GLM
```
---

## Structure

```bash
src/
 ├─ core/        # Renderer
 ├─ graphics/    # Shader / Texture
 ├─ objects/     # Mesh implementations (Cube)
 ├─ scene/       # Scene & Light management
include/
shaders/
```
---

## Current Architecture

Scene contains:
Objects (Mesh + Transform)
Light data (Dir / Point / Spot)
Renderer handles:
Camera matrices
Uniform upload
Draw calls
Mesh abstraction:
Cube implements Mesh interface
Status

```bash
✔ Texture rendering
✔ Lighting system (Phong)
✔ Camera movement
✔ Scene system
✔ Renderer abstraction
```
---

## Phase 1 – opengl tutorial c++ ( ~FIN~ )

- cube rendering
- texture mapping
- basic lighting
- camera system

## Phase 2 – Engine Completion (In Progress)

- Material system (Shader + Texture abstraction per object)
- Render queue system (Opaque / Transparent / Debug pass)
- Ownership cleanup (smart pointers / RAII transition)
- Batch rendering optimization

## Phase 3 – Ray Tracing Engine

- CPU-based ray tracer
- BVH acceleration structure
- Mesh intersection system
- Hybrid rasterization + ray tracing pipeline

## Phase 4 (GPU Acceleration)

- CUDA/OpenGL interop experiments
- Particle system simulation
- Compute shader-based rendering

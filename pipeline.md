# **3D Rendering Pipeline Specification (RenderPass-Based Architecture)**

This document describes the architecture, execution flow, and data lifecycle of the rendering system.
The engine adopts a **modular RenderPass pipeline**, separating rendering stages while preparing for **GPU-driven rendering extensions**.

---

## **1. System Architecture**

| Layer                      | Responsibility                                | Key Components                              |
| :------------------------- | :-------------------------------------------- | :------------------------------------------ |
| **Data (`include/`)**      | Scene representation and resource abstraction | `Scene`, `Mesh`, `Material`, `LightManager` |
| **Logic (`src/`)**         | Pipeline orchestration and scheduling         | `Renderer`, `RenderPasses`, `RenderQueue`   |
| **Execution (`shaders/`)** | GPU-side transformation and shading           | `basic.vert/frag`, `depth.vert/frag`        |

---

## **2. Rendering Pipeline Overview**

The engine follows a **multi-pass forward rendering pipeline**:

```text
Depth Pass (Shadow Map)
        ↓
Forward Pass (Lighting + Shadow)
        ↓
Light Pass (Debug / Visualization)
```

### **Design Rationale**

* **Decoupling**: Each pass is independently extendable
* **Scalability**: Enables insertion of compute / deferred passes
* **Future-ready**: Designed for GPU-driven rendering (SSBO + Indirect Draw)

---

## **3. Execution Flow**

### **Phase I — Initialization**

Performed once at startup:

1. **Mesh & Resource Loading**

   ```cpp
   ResourceManager::loadMesh();
   Mesh::setupMesh();
   ```

2. **Renderer Initialization**

   ```cpp
   Renderer::init();
   ```

   * Allocate UBOs (Camera, Lights)
   * Create shadow framebuffer (FBO + depth texture)

3. **Scene Construction**

   ```cpp
   Scene::addRenderGroup();
   ```

---

### **Phase II — Per-Frame Update**

#### **1. Scene Update (CPU)**

```cpp
Scene::update(dt);
```

#### **2. Light Synchronization (CPU → GPU)**

```cpp
LightManager::updateGPU();
```

* Uses **dirty flags**
* Upload via `glBufferSubData` (UBO)

#### **3. Render Queue Construction**

```cpp
Renderer::buildRenderQueue(scene);
```

Each object becomes:

```cpp
struct RenderItem {
    Mesh* mesh;
    Material* material;
    glm::mat4 model;
};
```

#### **4. Queue Optimization**

```cpp
Renderer::sortQueue();
```

Sort key:

```text
Material → Mesh
```

👉 Reduces:

* Shader switches
* Texture bindings
* Pipeline stalls

---

## **4. RenderPass Execution**

### **4.1 Depth Pass (Shadow Mapping)**

Generates shadow map from directional light:

```cpp
glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
glClear(GL_DEPTH_BUFFER_BIT);
```

Key operation:

```cpp
renderer.executeDrawObjects(scene, depthShader);
```

Matrix:

```cpp
lightSpaceMatrix = projection * view;
```

---

### **4.2 Forward Pass (Main Rendering)**

```cpp
glBindFramebuffer(GL_FRAMEBUFFER, 0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

Steps:

1. Bind camera UBO
2. Bind light UBO
3. Bind shadow map
4. Execute batched draw calls

```cpp
renderer.drawBatchedItems();
```

Batching strategy:

```text
(material + mesh) → instanced draw
```

---

### **4.3 Light Pass (Debug Visualization)**

```cpp
for (auto& light : scene.lightVisuals)
{
    light.mesh->draw();
}
```

Purpose:

* Visual debugging
* Light position validation

---

## **5. Data Flow & Memory Model**

```text
[ CPU ]                             [ GPU ]
------------------------------------------------------------
RenderQueue (vector)        →       Instance VBO
LightManager (structs)      →       UBO (std140)
Material params             →       Uniforms
Mesh data                   →       Static VBO/EBO
Shadow Map                  →       Depth Texture
```

---

## **6. Shader Pipeline**

### **Vertex Shader (`basic.vert`)**

Responsibilities:

* Apply instance transform
* Compute world position
* Generate shadow coordinates

```glsl
FragPosLightSpace = lightSpaceMatrix * model * vec4(position, 1.0);
gl_Position = projection * view * model * vec4(position, 1.0);
```

---

### **Fragment Shader (`basic.frag`)**

#### **Shadow Mapping**

```glsl
projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
shadow = texture(shadowMap, projCoords.xy);
```

#### **Lighting Model**

* Ambient
* Diffuse (Lambert)
* Specular (Blinn-Phong)

Final output:

```glsl
FragColor = vec4(ambient + shadow * (diffuse + specular), 1.0);
```

---

## **7. Optimization Techniques**

### **1. Instanced Rendering**

* Reduces draw calls significantly
* Uses per-instance matrix buffer

### **2. UBO Synchronization**

* Updates only when dirty
* Avoids redundant GPU transfers

### **3. Render Queue Sorting**

* Minimizes GPU state changes
* Improves cache locality

---

## **8. Current Limitations**

### **Rendering**

* CPU-driven pipeline
* No indirect draw
* No GPU culling

### **Shadow**

* Hard shadow only
* Single directional light
* No PCF filtering

### **Material System**

* Partial abstraction
* No automatic binding system

---

## **9. Roadmap**

### **Phase 2 (Current)**

* [ ] PCF soft shadows
* [ ] Material system refactor

---

### **Phase 3 (Critical Upgrade)** 🚀

* [ ] SSBO Render Queue
* [ ] Compute Shader Culling
* [ ] glMultiDrawIndirect

👉 Goal:

```text
CPU-driven → GPU-driven pipeline
```

---

### **Phase 4 (Advanced Rendering)**

* [ ] Deferred Rendering
* [ ] Clustered Lighting
* [ ] Hybrid CPU-GPU renderer

---

## **10. Key Insight**

This project is transitioning from:

```text
Basic OpenGL Renderer
        ↓
Structured Rendering Pipeline
        ↓
GPU-Driven Rendering System (Target)
```

---

## **Project Statement**

**This renderer is designed as a stepping stone toward GPU-driven rendering pipelines, emphasizing data-oriented design and minimizing CPU-GPU synchronization overhead.**

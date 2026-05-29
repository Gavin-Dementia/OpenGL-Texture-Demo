# Issue: 

------
## Missing gl_BaseInstance in OpenGL 4.3

During implementation of the GPU-driven indirect rendering pipeline, we attempted to use:

gl_BaseInstance
per-draw object indexing in vertex shader

However, the current runtime context is:

- OpenGL 4.3 Core Profile

### In OpenGL 4.3:

gl_BaseInstance is NOT available
gl_DrawID is NOT available
Only basic gl_InstanceID semantics are supported for instanced rendering
------
### As a result, shader compilation fails when using:

gl_BaseInstance
- Impact on Rendering Design

The original architecture assumed:

- Each indirect draw command encodes a unique baseInstance
- Vertex shader retrieves object index via gl_BaseInstance

However, due to API limitations, we cannot access per-draw command ID inside the vertex shader.
-------
### Current Implementation (OpenGL 4.3 Compatible)

We fall back to a simplified but stable mapping:

Object indexing strategy

Instead of:

objectID = gl_BaseInstance;

We use:

- objectID = gl_InstanceID;
- Indirect command configuration

Each command is generated as:

commands[drawID].instanceCount = 1;
commands[drawID].baseInstance = drawID; // reserved but NOT used in shader
---

### Vertex shader model

Object data is accessed directly via:

Instance inst = instances[gl_InstanceID];
mat4 model = transforms[inst.transformID];
------
### Limitation of Current Approach

This workaround introduces the following constraints:

- No true per-draw object ID separation via baseInstance
- Draw-level metadata is not accessible in vertex shader
- Multi-draw semantic abstraction is partially CPU/GPU hybrid instead of fully GPU-driven

However:

- GPU culling remains functional
- Indirect draw execution works correctly
- SSBO-based scene storage remains valid
- Pipeline architecture is stable
------

## Summary
- Current limitation is not architectural, but API-version dependent.


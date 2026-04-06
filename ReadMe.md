# OpenGL Texture Rendering Demo

## Features

* OpenGL 3.3 Core Profile
* Texture mapping (multi-texture blending)
* Shader abstraction (C++ class)
* stb_image image loading

## Demo

Render a textured rectangle using two textures (frog & wall).

## Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Dependencies

* GLFW
* GLAD
* stb_image

## Structure

* src/: source code
* shaders/: GLSL shaders
* include/: headers

## Future Work

* Camera system
* Lighting (Phong / Blinn-Phong)
* Model loading (OBJ)
* Integration with custom ThreadPool

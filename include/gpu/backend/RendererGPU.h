#ifndef RENDERER_GPU_H
#define RENDERER_GPU_H
#include <glad/glad.h>
#include "graphics/Shader.h"

class SceneUploader;
// class VisibilitySystem;
class VisibilityPipeline;

class RendererGPU
{
public:
    RendererGPU() = default;

    void init();

    void render(SceneUploader& scene,
        VisibilityPipeline& visibility,
        GLuint vao);

    void setDebugDraw(bool enable) { debugDraw = enable; }

private:
    void drawIndirect(
        GLuint vao,
        int drawCount);

    void drawIndirectDebug(
        GLuint vao,
        VisibilityPipeline& visibility,
        Shader& debugShader);

private:

    Shader normalShader;
    Shader debugShader;

    bool debugDraw = true;
    static constexpr int MAX_MESHES = 10000;
};

#endif // RENDERER_GPU_H


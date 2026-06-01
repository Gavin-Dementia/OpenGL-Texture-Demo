#ifndef RENDERER_GPU_H
#define RENDERER_GPU_H
#include <glad/glad.h>
#include "gpu/backend/BindingManager.h"
#include "graphics/Shader.h"
#include "gpu/passes/DrawPass.h"
#include "gpu/passes/VisibilityPass.h"
#include "gpu/frame/FrameContext.h"

#include "gpu/debug/GPUDebugValidator.h"
#include "gpu/debug/PipelineStateTracker.h"

class SceneUploader;

class RendererGPU
{
public:
    RendererGPU() = default;

    void init();

    void render(Scene& scene,
                SceneUploader& uploader,
                BindingManager& binding,
                DrawPass& draw,
                GLuint vao);

private:
    void drawIndirect(int drawCount);

    void drawIndirectDebug(
        GLuint vao,
        Shader& debugShader){};

private:

    Shader normalShader;
    Shader debugShader;
    Shader computeShader;

    GPUDebugValidator debug;
    PipelineStateTracker tracker;

    bool debugDraw = true;
    static constexpr int MAX_DRAWS = 10000;
};

#endif // RENDERER_GPU_H


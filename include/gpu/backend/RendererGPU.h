#ifndef RENDERER_GPU_H
#define RENDERER_GPU_H
#include <glad/glad.h>
#include "gpu/backend/BindingManager.h"
#include "graphics/Shader.h"
#include "gpu/pipeline/VisibilityPipeline.h"
#include "gpu/pipeline/DrawPipeline.h"
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

    void render(
        Scene& scene,
        SceneUploader& uploader,
        VisibilityPass& visibility,
        DrawPass& draw,
        BindingManager& binding,
        GLuint vao);

    // void setDebugDraw(bool enable) { debugDraw = enable; }
    DrawPipeline& getDrawPipeline() { return drawPipeline; }

private:
    void drawIndirect(int drawCount);

    void drawIndirectDebug(
        GLuint vao,
        VisibilityPipeline& visibility,
        Shader& debugShader){};

private:

    Shader normalShader;
    Shader debugShader;
    Shader computeShader;

    VisibilityPipeline visibilityPipeline;
    DrawPipeline drawPipeline;

    GPUDebugValidator debug;
    PipelineStateTracker tracker;

    bool debugDraw = true;
    static constexpr int MAX_MESHES = 10000;
};

#endif // RENDERER_GPU_H


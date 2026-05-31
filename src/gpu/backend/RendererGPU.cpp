#include "gpu/backend/RendererGPU.h"
#include "gpu/backend/BindingManager.h"
#include "gpu/upload/SceneUploader.h"
#include "gpu/pipeline/VisibilityPipeline.h"
#include "gpu/pipeline/DrawPipeline.h"
#include "graphics/Shader.h"
#include "core/ShaderLoader.h"
#include "scene/Scene.h"
#include "gpu/frame/FrameContext.h"
#include "gpu/passes/DrawPass.h"
#include "gpu/passes/VisibilityPass.h"
#include "gpu/layout/Binding.h"

#include <iostream>

#include "gpu/commands/DrawElementsIndirectCommandBuffer.h"

void RendererGPU::init()
{
    std::cout << "RendererGPU init\n";
}

void RendererGPU::render(
    Scene& scene,
    SceneUploader& uploader,
    VisibilityPass& visibility,
    DrawPass& draw,
    BindingManager& binding,
    GLuint vao)
{
    FrameContext frame{};

    debug.beginFrame();

    // =========================================================
    // 0. CPU INSTANCE COUNT SNAPSHOT
    frame.instanceCount = uploader.getInstanceBuffer().size();

    std::cout << "[Frame] instanceCount = "
              << frame.instanceCount << std::endl;

    // =========================================================
    // 1. RESET GPU COUNTER (drawCount + instanceCount struct)
    uploader.getCounterBuffer().reset();

    // IMPORTANT: upload instanceCount BEFORE compute
    uploader.getCounterBuffer().uploadInstanceCount(frame.instanceCount);

    std::cout << "[Frame] counter reset + instanceCount uploaded" << std::endl;

    // =========================================================
    // 2. BIND FOR COMPUTE (FULL FRAME STATE)
    binding.bindForCompute(uploader);

    // =========================================================
    // 3. VISIBILITY PASS (COMPUTE)
    visibility.execute(frame, scene);

    std::cout << "[Frame] compute finished" << std::endl;

    // =========================================================
    // 4. GPU SYNC (ONLY ONCE)
    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT |
        GL_BUFFER_UPDATE_BARRIER_BIT
    );

    std::cout << "[Frame] memory barrier done" << std::endl;

    // =========================================================
    // 5. READBACK (DEBUG ONLY, NOT DRIVING GPU LOGIC)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,
                     uploader.getCounterBuffer().getID());
        std::cout << "[DEBUG] scene.instances = "
            << scene.instances.size() << std::endl;

        struct GPUCounter
        {
            GLuint drawCount;
            GLuint instanceCount;
        } counter{};

        glGetBufferSubData(
            GL_SHADER_STORAGE_BUFFER,
            0,
            sizeof(GPUCounter),
            &counter
        );

        frame.drawCount = counter.drawCount;
        frame.instanceCount = counter.instanceCount;

        std::cout << "[Frame] GPU drawCount = "
                  << frame.drawCount << std::endl;

        std::cout << "[Frame] GPU instanceCount = "
                  << frame.instanceCount << std::endl;
    }

    // =========================================================
    // 6. DRAW BINDING PHASE
    binding.bindForDraw(uploader);

    draw.bind(
        vao,
        uploader.getDrawBuffer(),
        uploader.getCounterBuffer()
    );

    // =========================================================
    // 7. INDIRECT DRAW EXECUTION
    if (frame.drawCount > 0)
    {
        std::cout << "[Frame] draw execute = "
                  << frame.drawCount << std::endl;

        draw.execute(frame.drawCount);
    }
    else
    {
        std::cout << "[Frame] skip draw (0)" << std::endl;
    }

    // =========================================================
    // 8. END FRAME
    debug.endFrame();

    std::cout << "[Frame] end\n" << std::endl;
}

void RendererGPU::drawIndirect(int drawCount)
{
    std::cout << "[RendererGPU] drawIndirect legacy call\n";

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,
        GL_UNSIGNED_INT,
        nullptr,
        drawCount,
        0
    );
}


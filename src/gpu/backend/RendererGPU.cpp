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
    FrameContext frame;

    debug.beginFrame();

    // GLuint instanceCount = scene.getInstanceCount();
    GLuint instanceCount = uploader.getInstanceBuffer().size();
    GLuint groups = (instanceCount + 63) / 64;
    std::cout << "uploader.getInstanceBuffer().size() = " << instanceCount << std::endl;
    std::cout << "groups = " << groups << std::endl;

    tracker.setDispatch(groups);

    uploader.getCounterBuffer().reset(0);
    // =========================
    // 1. COMPUTE BINDING PHASE
    binding.bindForCompute(uploader);

    std::cout << "===== SSBO BINDINGS =====\n";

    for (int i = 0; i <= 11; i++)
    {
        GLint buf = 0;
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, i, &buf);

        std::cout << "binding[" << i << "] = " << buf << "\n";
    }
    
    // =========================
    // 2. COMPUTE PASS
    visibility.execute(
        scene,
        uploader,
        frame.drawCount
    );

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                GL_COMMAND_BARRIER_BIT |
                GL_BUFFER_UPDATE_BARRIER_BIT);

    // =========================
    // 3. VALIDATION AFTER COMPUTE
    debug.validateComputeDispatch(scene.getInstanceCount());

    tracker.setDrawCount(frame.drawCount);
    debug.validateDrawCount(frame.drawCount);
    // =========================
    // 4. DRAW BINDING PHASE
    binding.bindForDraw(uploader);
    draw.setVAO(vao);

    // DrawElementsIndirectCommandBuffer cmd;
    // glGetBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(cmd), &cmd);
    // std::cout <<"cmd.count = " << cmd.count << std::endl;
    // std::cout <<"cmd.instanceCount = " << cmd.instanceCount << std::endl;
    // std::cout <<"cmd.firstIndex = " << cmd.firstIndex << std::endl;
    // std::cout <<"cmd.baseVertex = " << cmd.baseVertex << std::endl;
    // std::cout <<"cmd.baseInstance = " << cmd.baseInstance << std::endl;
    // std::cout << "frame.drawCount = " << frame.drawCount << "\n";
    
    // =========================
    // 5. INDIRECT DRAW
    draw.execute(
        uploader.getDrawBuffer(),
        frame.drawCount
    );
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

    // =========================
    // 6. FINAL VALIDATION
    debug.validateIndirectBuffer(uploader.getDrawBuffer().getID());

    debug.endFrame();
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


#include "gpu/passes/VisibilityPass.h"
#include "gpu/upload/SceneUploader.h"
#include "gpu/buffers/DrawBuffer.h"
#include "scene/Scene.h"
#include "core/ShaderLoader.h"

#include <glad/glad.h>
// #include <iostream>

void VisibilityPass::init()
{
    computeProgram = ShaderLoader::loadCompute("cull.comp");
}

void VisibilityPass::execute(FrameContext& frame, Scene& scene)
{
    computeProgram.use();

    frame.dispatchGroups =
        (scene.getInstanceCount() + 63) / 64;

    glDispatchCompute(frame.dispatchGroups, 1, 1);

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT |
        GL_ATOMIC_COUNTER_BARRIER_BIT |
        GL_BUFFER_UPDATE_BARRIER_BIT
    );
}


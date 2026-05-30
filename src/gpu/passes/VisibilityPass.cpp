#include "gpu/passes/VisibilityPass.h"
#include "gpu/upload/SceneUploader.h"
#include "gpu/buffers/DrawBuffer.h"
#include "scene/Scene.h"
#include "core/ShaderLoader.h"

#include <glad/glad.h>
#include <iostream>

void VisibilityPass::init()
{
    computeProgram = ShaderLoader::loadCompute("cull.comp");
}

void VisibilityPass::execute(
    Scene& scene,
    SceneUploader& uploader,
    GLuint& outDrawCount)
{
    computeProgram.use();

    GLuint instanceCount = scene.getInstanceCount();
    GLuint groups = (instanceCount + 63) / 64;

    glDispatchCompute(groups, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                GL_COMMAND_BARRIER_BIT |
                GL_BUFFER_UPDATE_BARRIER_BIT);
                
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,
                 uploader.getCounterBuffer().getID());

    glGetBufferSubData(
        GL_SHADER_STORAGE_BUFFER,
        0,
        sizeof(GLuint),
        &outDrawCount
    );
}


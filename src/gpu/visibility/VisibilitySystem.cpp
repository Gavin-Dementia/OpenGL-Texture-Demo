#include "gpu/visibility/VisibilitySystem.h"
#include "gpu/visibility/IndirectDraw.h"
#include "C:/3Dproject/src/core/ShaderLoader.h"

#include <iostream>

void VisibilitySystem::init()
{
    // indirect draw commands (input for renderer)
    indirectCommandBuffer.create(
        1024 * sizeof(DrawElementsIndirectCommand),
        5
    );
    
    // atomic counter (how many visible)
    counterBuffer.create(sizeof(GLuint), 6);

    // visible instances (output of culling)
    visibleInstanceBuffer.create(1024 * 1024, 8);

    GLuint zero = 0;
    counterBuffer.upload(&zero, sizeof(GLuint));

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                     5, indirectCommandBuffer.getID());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 
                     6, counterBuffer.getID());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
                     8, visibleInstanceBuffer.getID());

    computeProgram = ShaderLoader::loadCompute("cull.comp");
}

void VisibilitySystem::bindBuffers() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer.getID());
    unsigned int zero = 0;
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), &zero);
}

void VisibilitySystem::bindInputs(const GPUScene& scene)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
                     scene.getInstanceBuffer().getID());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
                     scene.getTransformBuffer().getID());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
                     scene.getMeshBuffer().getID());

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene.getCameraUBO());
}

void VisibilitySystem::bindOutputs() const
{
    indirectCommandBuffer.bindBase();
    visibleInstanceBuffer.bindBase();
    counterBuffer.bindBase();
}

void VisibilitySystem::dispatchCulling(
    GPUScene& gpuScene,
    int instanceCount)
{
    std::cout << "dispatch start\n";
    // glUseProgram(computeProgram);
    computeProgram.use();
    bindInputs(gpuScene);
    bindOutputs();

    // reset counter
    GLuint zero = 0;
    counterBuffer.upload(&zero, sizeof(GLuint));

    // dispatch
    glDispatchCompute((instanceCount + 63) / 64, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_COMMAND_BARRIER_BIT |
                    GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}


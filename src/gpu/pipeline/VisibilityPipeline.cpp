#include "gpu/pipeline/VisibilityPipeline.h"
#include "gpu/commands/IndirectCommandBuffer.h"
#include "C:/3Dproject/src/core/ShaderLoader.h"

#include <iostream>

void VisibilityPipeline::init()
{
    // atomic counter (how many visible)
    CounterBuffer.create(sizeof(GLuint), GL_DYNAMIC_DRAW);

    // visible instances (output of culling)
    VisibilityBuffer.create(1024 * 1024, GL_DYNAMIC_DRAW);

    GLuint zero = 0;
    CounterBuffer.upload(&zero, sizeof(GLuint));

    CounterBuffer.bindBase(6);
    VisibilityBuffer.bindBase(8);

    computeProgram = ShaderLoader::loadCompute("cull.comp");
}

void VisibilityPipeline::bindBuffers() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, CounterBuffer.getID());
    unsigned int zero = 0;
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), &zero);
}

void VisibilityPipeline::bindInputs(const SceneUploader& scene)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
                     scene.getObjectBuffer().getID());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
                     scene.getTransformBuffer().getID());

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene.getCameraUBO());
}

void VisibilityPipeline::bindOutputs(const SceneUploader& scene)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4,
                     scene.getDrawBuffer().getID());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8,
                     VisibilityBuffer.getID());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6,
                     CounterBuffer.getID());
}

void VisibilityPipeline::dispatch(
    SceneUploader& scene,
    int instanceCount)
{
    bindInputs(scene);
    bindOutputs(scene);

    computeProgram.use();

    // reset counter
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,
                 CounterBuffer.getID());

    GLuint zero = 0;
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    0,
                    sizeof(GLuint),
                    &zero);

    // dispatch compute
    GLuint groupSize = 64;
    GLuint groups = (instanceCount + groupSize - 1) / groupSize;
        std::cout << "instanceCount = " << instanceCount << std::endl;
        std::cout << "groups = " << groups << std::endl;
    glDispatchCompute(groups, 1, 1);

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT |
        GL_COMMAND_BARRIER_BIT
    );
}


void VisibilityPipeline::dispatchCulling(
    SceneUploader& gpuScene,
    int instanceCount)
{
    std::cout << "dispatch start\n";
    // glUseProgram(computeProgram);
    computeProgram.use();
    bindInputs(gpuScene);
    bindOutputs(gpuScene);

    // reset counter
    GLuint zero = 0;
    CounterBuffer.upload(&zero, sizeof(GLuint));

    // dispatch
    glDispatchCompute((instanceCount + 63) / 64, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_COMMAND_BARRIER_BIT |
                    GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}


#include "gpu/pipeline/VisibilityPipeline.h"
#include "gpu/commands/DrawElementsIndirectCommandBuffer.h"
#include "C:/3Dproject/src/core/ShaderLoader.h"

#include <iostream>

void VisibilityPipeline::init()
{
    // atomic counter (how many visible)
    counterBuffer.create(sizeof(GLuint), GL_DYNAMIC_DRAW);

    // visible instances (output of culling)
    // visibilityBuffer.create(1024 * 1024, GL_DYNAMIC_DRAW);

    GLuint zero = 0;
    counterBuffer.upload(&zero, sizeof(GLuint));

    // counterBuffer.bindBase(6);
    // visibilityBuffer.bindBase(8);

    computeProgram = ShaderLoader::loadCompute("cull.comp");
    std::cout << "[VisibilityPipeline] init done\n";
}

// void VisibilityPipeline::bindBuffers() const
// {
//     glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer.getID());
//     unsigned int zero = 0;
//     glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), &zero);
// }

void VisibilityPipeline::bindInputs(const SceneUploader& scene)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
                     scene.getObjectBuffer().getID());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
                     scene.getTransformBuffer().getID());

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene.getCameraUBO());
}

void VisibilityPipeline::bindOutputs(DrawBuffer& drawBuffer)
{
    // slot 4: indirect draw commands output
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4,
                     drawBuffer.getID());

glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawBuffer.getID());
    // slot 6: atomic counter
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6,
                     counterBuffer.getID());
}

// void VisibilityPipeline::bindOutputs(const SceneUploader& scene)
// {
//     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4,
//                      scene.getDrawBuffer().getID());
//     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8,
//                      visibilityBuffer.getID());
//     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6,
//                      counterBuffer.getID());
// }

// void VisibilityPipeline::dispatch(
//     SceneUploader& scene,
//     int instanceCount)
// {
//     bindInputs(scene);
//     bindOutputs(scene);

//     computeProgram.use();

//     // reset counter
//     glBindBuffer(GL_SHADER_STORAGE_BUFFER,
//                  counterBuffer.getID());

//     GLuint zero = 0;
//     glBufferSubData(GL_SHADER_STORAGE_BUFFER,
//                     0,
//                     sizeof(GLuint),
//                     &zero);

//     // dispatch compute
//     GLuint groupSize = 64;
//     GLuint groups = (instanceCount + groupSize - 1) / groupSize;
//         std::cout << "instanceCount = " << instanceCount << std::endl;
//         std::cout << "groups = " << groups << std::endl;
//     glDispatchCompute(groups, 1, 1);

//     glMemoryBarrier(
//         GL_SHADER_STORAGE_BARRIER_BIT |
//         GL_COMMAND_BARRIER_BIT
//     );
// }

void VisibilityPipeline::dispatch(
    const SceneUploader& scene,
    DrawBuffer& drawBuffer,
    GLuint& outDrawCount,
    int instanceCount)
{
    computeProgram.use();

    bindInputs(scene);
    bindOutputs(drawBuffer);

    GLuint zero = 0;
    counterBuffer.upload(&zero, sizeof(GLuint));

    GLuint groups = (instanceCount + 63) / 64;

    std::cout << "[VisibilityPipeline] dispatch compute\n";
    std::cout << "instanceCount = " << instanceCount << std::endl;
    std::cout << "groups = " << groups << std::endl;

    glDispatchCompute(groups, 1, 1);

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT |
        GL_COMMAND_BARRIER_BIT
    );

    // CPU readback ONLY for draw count
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer.getID());
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &outDrawCount);
}


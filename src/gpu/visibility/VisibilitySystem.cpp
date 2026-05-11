#include "gpu/visibility/VisibilitySystem.h"

#include <iostream>

void VisibilitySystem::init()
{
    // visible instances (output of culling)
    visibleInstanceBuffer.create(1024 * 1024, 4);

    // indirect draw commands
    indirectBuffer.create(1024 * 1024, 5);

    // atomic counter (how many visible)
    counterBuffer.create(sizeof(GLuint), 6);

    GLuint zero = 0;
    counterBuffer.upload(&zero, sizeof(GLuint));
}

void VisibilitySystem::bindBuffers() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer.getID());
    unsigned int zero = 0;
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), &zero);
}

void VisibilitySystem::dispatchCulling(
    GLuint instanceBuffer,
    GLuint transformBuffer,
    GLuint meshBuffer,
    GLuint cameraUBO,
    int instanceCount)
{
    glUseProgram(computeProgram);

    // bind input
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instanceBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, transformBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, meshBuffer);

    // output
    visibleInstanceBuffer.bindBase();
    indirectBuffer.bindBase();
    counterBuffer.bindBase();

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);

    // reset counter
    GLuint zero = 0;
    counterBuffer.upload(&zero, sizeof(GLuint));

    // dispatch
    glDispatchCompute((instanceCount + 63) / 64, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_COMMAND_BARRIER_BIT |
                    GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}


#include "gpu/backend/RendererGPU.h"
#include "gpu/scene/GPUScene.h"
#include "gpu/visibility/VisibilitySystem.h"
#include "graphics/Shader.h"
#include "core/ShaderLoader.h"

#include <iostream>
#define SHADER_PATH "../shaders/"

void RendererGPU::init()
{
    normalShader = ShaderLoader::load("basic.vert", "basic.frag");

    debugShader = ShaderLoader::load("indirect_debug.vert", "indirect_debug.frag");

    std::cout << "RendererGPU init\n";
}

void RendererGPU::render(
    GPUScene& scene,
    VisibilitySystem& visibility,
    GLuint vao)
{
    std::cout << "RendererGPU render\n";
    scene.bindAll();
    visibility.bindOutputs();

    glBindVertexArray(vao);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
                 visibility.getIndirectBuffer());

    // glBindBuffer(GL_PARAMETER_BUFFER,
    //              visibility.getCounterBuffer());4.6

    normalShader.use();

    // 1. GPU → CPU sync（counter）
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT |
                    GL_SHADER_STORAGE_BARRIER_BIT);

    GLuint count = 0;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER,
                 visibility.getCounterBuffer());
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER,
                        0, 
                        sizeof(GLuint),
                        &count);
                        
    std::cout << "visible count = " << count << std::endl;
    if (count == 0)
    {
        glBindVertexArray(0);
        return;
    }

    // 2. draw
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT);

    // glMultiDrawElementsIndirectCount(
    //     GL_TRIANGLES,
    //     GL_UNSIGNED_INT,
    //     nullptr,
    //     0,
    //     MAX_MESHES,
    //     0
    // );4.6

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,
        GL_UNSIGNED_INT,
        nullptr,
        count,
        0
    );

    std::cout << "draw count = " << count << std::endl;

    glBindVertexArray(0);
}

void RendererGPU::drawIndirect(
    GLuint vao,
    int drawCount)
{
    glBindVertexArray(vao);

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,
        GL_UNSIGNED_INT,
        nullptr,
        drawCount,
        0);

    glBindVertexArray(0);
}

void RendererGPU::drawIndirectDebug(
    GLuint vao,
    VisibilitySystem& visibility,
    Shader& debugShader)
{
    debugShader.use();

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
                 visibility.getIndirectBuffer());

    glBindVertexArray(vao);

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,
        GL_UNSIGNED_INT,
        nullptr,
        MAX_MESHES,
        0);

    glBindVertexArray(0);
}
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
    GLuint vao,
    int drawCount)
{
    scene.bindAll();

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
                 visibility.getIndirectBuffer());

    glBindVertexArray(vao);

    if (debugDraw)
    {
        drawIndirectDebug(vao, visibility, debugShader);
    }
    else
    {
        normalShader.use();

        glMultiDrawElementsIndirect(
            GL_TRIANGLES,
            GL_UNSIGNED_INT,
            nullptr,
            drawCount,
            0);
    }

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
#include "gpu/passes/DrawPass.h"
#include "gpu/buffers/DrawBuffer.h"
#include "core/ShaderLoader.h"

#include <glad/glad.h>

void DrawPass::init()
{
    shader = ShaderLoader::loadPass("basic.vert", "basic.frag");
}

void DrawPass::setVAO(GLuint inVao)
{
    vao = inVao;
}

void DrawPass::execute(
    const DrawBuffer& drawBuffer,
    GLuint drawCount)
{
    if (drawCount == 0) return;

    shader.use();
    glBindVertexArray(vao);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawBuffer.getID());

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                GL_COMMAND_BARRIER_BIT |
                GL_BUFFER_UPDATE_BARRIER_BIT);

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,
        GL_UNSIGNED_INT,
        nullptr,
        drawCount,
        0
    );
}


#include "gpu/passes/DrawPass.h"
#include "gpu/buffers/DrawBuffer.h"
#include "gpu/buffers/CounterBuffer.h"
#include "core/ShaderLoader.h"
#include "graphics/Shader.h"

#include <iostream>

void DrawPass::init()
{
    shader = ShaderLoader::loadPass("basic.vert", "basic.frag");
}

void DrawPass::bind(GLuint vao_,
                    const DrawBuffer& drawBuffer,
                    const CounterBuffer& counterBuffer)
{
    vao = vao_;
    drawBufferID = drawBuffer.getID();
    counterBufferID = counterBuffer.getID();
}

void DrawPass::execute(GLuint drawCount)
{
    shader.use();
    glBindVertexArray(vao);

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,
        GL_UNSIGNED_INT,
        nullptr,
        drawCount,
        0
    );
}


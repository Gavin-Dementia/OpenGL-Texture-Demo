#include "gpu/pipeline/DrawPipeline.h"
#include "gpu/buffers/DrawBuffer.h"
#include "core/ShaderLoader.h"

#include <glad/glad.h>
#include <iostream>

void DrawPipeline::init()
{   
    drawShader = ShaderLoader::loadPass("basic.vert", "basic.frag"); 
}

void DrawPipeline::setVAO(GLuint inVao)
{
    vao = inVao;
}

void DrawPipeline::render(const DrawBuffer& indirectBuffer, GLuint drawCount)
{
    if (drawCount == 0)    return;    
    
    drawShader.use();

    glBindVertexArray(vao);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer.getID());

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT |
        GL_COMMAND_BARRIER_BIT
    );

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,
        GL_UNSIGNED_INT,
        nullptr,
        drawCount,
        0
    );
}


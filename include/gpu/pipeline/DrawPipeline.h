#ifndef DRAW_PIPELINE_H
#define DRAW_PIPELINE_H
#include <glad/glad.h>

#include "graphics/Shader.h"
#include "gpu/buffers/DrawBuffer.h"

class DrawPipeline
{
public:
    void init();
    void setVAO(GLuint inVao);
    void render(const DrawBuffer& indirectBuffer, GLuint drawCount);

private:
    Shader drawShader;
    GLuint drawCount;
    GLuint vao;
};

#endif // DRAW_PIPELINE_H


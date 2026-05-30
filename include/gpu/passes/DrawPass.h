#ifndef DRAW_PASS_H
#define DRAW_PASS_H

#include <glad/glad.h>
#include "graphics/Shader.h"

class DrawBuffer;

class DrawPass
{
public:

    void init();
    void setVAO(GLuint vao);

    void execute(
        const DrawBuffer& drawBuffer,
        GLuint drawCount);

private:

    Shader shader;
    GLuint vao = 0;
};

#endif // DRAW_PASS_H

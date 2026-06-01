#ifndef DRAW_PASS_H
#define DRAW_PASS_H

#include <glad/glad.h>
#include "graphics/Shader.h"

class DrawBuffer;
class CounterBuffer;
class Shader;

class DrawPass
{
public:
    void init();

    // bind GPU resources once per frame (optional)
    void bind(GLuint vao,
              const DrawBuffer& drawBuffer,
              const CounterBuffer& counterBuffer);

    // GPU-driven draw (no CPU drawCount)
    void execute(GLuint drawCount);

private:
    Shader shader;

    GLuint vao = 0;
    GLuint drawBufferID = 0;
    GLuint counterBufferID = 0;

};

#endif


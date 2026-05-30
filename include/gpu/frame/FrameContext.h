#ifndef FRAME_CONTEXT_H
#define FRAME_CONTEXT_H
#include <glad/glad.h>

class FrameContext
{
public:
    void beginFrame();
    void endFrame();

    GLuint drawCount = 0;
    GLuint frameIndex = 0;
};

#endif // FRAME_CONTEXT_H


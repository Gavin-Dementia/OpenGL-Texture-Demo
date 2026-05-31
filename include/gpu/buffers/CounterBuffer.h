#ifndef COUNTER_BUFFER_H
#define COUNTER_BUFFER_H
#include <glad/glad.h>
#include "gpu/buffers/SSBO.h"

struct GPUCounter
{
    GLuint drawCount;
    GLuint instanceCount;
};

class CounterBuffer
{
public:
    void init();
    void reset();
    void bind() const;

    void uploadInstanceCount(GLuint count);

    GLuint getID() const;
    GLuint read() const;

private:
    GLuint id = 0;
    SSBO ssbo;
};

#endif // COUNTER_BUFFER_H


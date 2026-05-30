#ifndef COUNTER_BUFFER_H
#define COUNTER_BUFFER_H
#include <glad/glad.h>
#include "gpu/buffers/SSBO.h"

class CounterBuffer
{
public:
    void init();
    void reset(GLuint value);
    void bind() const;

    GLuint getID() const;

private:
    GLuint id = 0;
    SSBO ssbo;
};

#endif // COUNTER_BUFFER_H


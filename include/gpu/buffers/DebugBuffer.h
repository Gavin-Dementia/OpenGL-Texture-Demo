#ifndef DEBUG_BUFFER_H
#define DEBUG_BUFFER_H
#include <glad/glad.h>
#include "gpu/buffers/SSBO.h"

class DebugBuffer
{
public:
    void init(GLsizeiptr maxCount);
    void upload(const void* data, size_t size);
    void bind() const;

    GLuint getID() const;

private:
    SSBO ssbo;
};

#endif // DEBUG_BUFFER_H


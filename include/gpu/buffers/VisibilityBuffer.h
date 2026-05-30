#ifndef VISIBILITY_BUFFER_H
#define VISIBILITY_BUFFER_H
#include <glad/glad.h>
#include "gpu/buffers/SSBO.h"

class VisibilityBuffer
{
public:
    void init(GLsizeiptr maxCount);
    void clear();
    void bind() const;

    GLuint getID() const;

private:
    SSBO ssbo;
    size_t maxCount = 0;
};

#endif // VISIBILITY_BUFFER_H


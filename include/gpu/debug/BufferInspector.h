#ifndef BufferInspector_H
#define BufferInspector_H
#include <glad/glad.h>

class BufferInspector
{
public:
    GLuint peekUint(GLuint buffer, GLuint offset = 0);
    GLuint peekIndirectCount(GLuint buffer);
};

#endif // BufferInspector_H


#include "gpu/debug/BufferInspector.h"
#include <iostream>

GLuint BufferInspector::peekUint(GLuint buffer, GLuint offset)
{
    GLuint value = 0;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(GLuint), &value);

    return value;
}

GLuint BufferInspector::peekIndirectCount(GLuint buffer)
{
    struct Cmd
    {
        GLuint count;
        GLuint instanceCount;
        GLuint firstIndex;
        GLuint baseVertex;
        GLuint baseInstance;
    } cmd;

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer);
    glGetBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(Cmd), &cmd);

    return cmd.count;
}
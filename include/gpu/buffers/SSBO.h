#ifndef SSBO_H
#define SSBO_H
#include <glad/glad.h>
#include <cstddef>

class SSBO
{
public:

    SSBO();
    ~SSBO();

    void create(
        size_t size,
        GLuint binding,
        GLenum usage = GL_DYNAMIC_DRAW);

    void upload(
        const void* data,
        size_t size,
        size_t offset = 0);

    void bind() const;
    void bindBase() const;
    void destroy();

    GLuint getID() const;

    size_t getSize() const;

private:

    GLuint id = 0;
    GLuint binding = 0;
    GLuint debugSSBO;

    size_t bufferSize = 0;
};

#endif // SSBO_H


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
        GLenum usage = GL_DYNAMIC_DRAW);

    void upload(
        const void* data,
        size_t size,
        size_t offset = 0);

    void bindBase() const;
    void bindBase(GLuint binding) const;
    void destroy();

    GLuint getID() const;

    size_t getSize() const;

private:

    GLuint id = 0;
    size_t bufferSize = 0;
    
    GLuint binding = 0;
    GLuint debugSSBO;
};

#endif // SSBO_H


#ifndef INSTANCEBUFFER_H
#define INSTANCEBUFFER_H
#include <glad/glad.h>
#include "gpu/buffers/SSBO.h"

struct Instance
{
    GLuint transformID;
    GLuint meshID;
    GLuint materialID;
    GLuint visibilityID;
};

class InstanceBuffer
{
public:
    void init(GLsizeiptr maxInstances);
    void upload(const Instance* data, size_t count_);
    void bind() const;

    GLuint getID() const;
    GLuint size() const { return count; }

private:
    SSBO ssbo;

    GLuint count = 0;
    size_t maxCount = 0;
};

#endif // INSTANCEBUFFER_H


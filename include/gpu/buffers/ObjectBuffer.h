#ifndef OBJECTBUFFER_H
#define OBJECTBUFFER_H

#include <glad/glad.h>
#include <vector>

#include "gpu/buffers/SSBO.h"

class GPUObjectData;

class ObjectBuffer
{
public:
    static constexpr GLuint Binding= 0;

    void init();

    void upload(const std::vector<GPUObjectData>& objects);

    void bind() const;

    size_t size() const;

    GLuint getID() const
    {  return ssbo.getID();  }

private:

    SSBO ssbo;
};
#endif // OBJECTBUFFER_H


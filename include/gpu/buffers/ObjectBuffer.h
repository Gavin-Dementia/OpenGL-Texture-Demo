#ifndef OBJECTBUFFER_H
#define OBJECTBUFFER_H

#include <glad/glad.h>
#include <vector>

#include "gpu/buffers/SSBO.h"

class GPUObjectData;

class ObjectBuffer
{
public:

    void init();

    void upload(const std::vector<GPUObjectData>& objects);

    void bind(GLuint binding) const;

    size_t size() const;

private:

    SSBO ssbo;
};
#endif // OBJECTBUFFER_H


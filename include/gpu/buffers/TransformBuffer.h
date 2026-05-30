#ifndef TRANSFORMBUFFER_H
#define TRANSFORMBUFFER_H

#include <glad/glad.h>
#include <vector>

#include "gpu/buffers/SSBO.h"

class GPUTransformData;

class TransformBuffer
{
public:
    void init();
    void upload(const std::vector<GPUTransformData>& transforms);
    void bind() const;

    GLuint size() const;

    GLuint getID() const
    {  return ssbo.getID();  }

private:

    SSBO ssbo;
};

#endif // TRANSFORMBUFFER_H


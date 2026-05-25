#ifndef MATERIALBUFFER_H
#define MATERIALBUFFER_H

#include <glad/glad.h>
#include <vector>

#include "gpu/buffers/SSBO.h"

class GPUMaterialData;

class MaterialBuffer
{
public:

    static constexpr GLuint Binding = 1;

    void init();

    void upload(
        const std::vector<GPUMaterialData>& materials);

    void bind() const;

    size_t size() const;
    
    GLuint getID() const
    {  return ssbo.getID();  }

private:

    SSBO ssbo;
};

#endif // MATERIALBUFFER_H
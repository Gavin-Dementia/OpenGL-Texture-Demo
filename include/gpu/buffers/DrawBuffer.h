#ifndef DRAWBUFFER_H
#define DRAWBUFFER_H

#include <glad/glad.h>
#include <vector>

#include "gpu/buffers/SSBO.h"
#include "gpu/layout/MeshDrawData.h"


class DrawBuffer
{
public:

    static constexpr GLuint Binding = 5;

    void init();

    void upload(
        const std::vector<MeshDrawData>& commands);

    void bind() const;

    size_t size() const;
 
    GLuint getID() const
    {  return ssbo.getID();  }

private:

    SSBO ssbo;
};

#endif // DRAWBUFFER_H


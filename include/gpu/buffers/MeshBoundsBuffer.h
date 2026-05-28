#ifndef MESH_BOUNDS_BUFFER_H
#define MESH_BOUNDS_BUFFER_H
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

#include "gpu/buffers/SSBO.h"

class MeshBoundsBuffer
{
public:
    void init();
    void upload(const std::vector<glm::vec4>& bounds);
    void bind() const;

    GLuint getID() const;

private:
    SSBO ssbo;
};

#endif // MESH_BOUNDS_BUFFER_H


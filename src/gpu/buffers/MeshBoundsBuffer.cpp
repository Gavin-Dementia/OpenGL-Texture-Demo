#include "gpu/buffers/MeshBoundsBuffer.h"
#include "gpu/layout/Binding.h"

void MeshBoundsBuffer::init()
{
    ssbo.create(1024 * sizeof(glm::vec4), GL_DYNAMIC_DRAW);
}

void MeshBoundsBuffer::upload(const std::vector<glm::vec4>& bounds)
{
    ssbo.upload(bounds.data(), bounds.size() * sizeof(glm::vec4));
}

void MeshBoundsBuffer::bind() const
{    ssbo.bindBase(Binding::SSBO::MeshBoundsBuffer);  }

GLuint MeshBoundsBuffer::getID() const
{    return ssbo.getID();  }




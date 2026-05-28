#include "gpu/buffers/DrawBuffer.h"
#include "gpu/layout/Binding.h"
#include "gpu/layout/MeshDrawData.h"

void DrawBuffer::init()
{
    ssbo.create(
        sizeof(MeshDrawData),
        GL_DYNAMIC_DRAW);
}

void DrawBuffer::upload(
    const std::vector<MeshDrawData>& commands)
{
    if (commands.empty())
        return;

    ssbo.upload(
        commands.data(),
        sizeof(MeshDrawData) * commands.size());
}

void DrawBuffer::bind() const
{    ssbo.bindBase(Binding::SSBO::DrawBuffer);  }

size_t DrawBuffer::size() const
{    return ssbo.getSize() / sizeof(MeshDrawData);  }


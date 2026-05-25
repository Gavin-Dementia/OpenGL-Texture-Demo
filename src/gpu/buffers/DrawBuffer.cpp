#include "gpu/buffers/DrawBuffer.h"

#include "gpu/layout/DrawCommandData.h"

void DrawBuffer::init()
{
    ssbo.create(
        sizeof(DrawCommandData),
        GL_DYNAMIC_DRAW);
}

void DrawBuffer::upload(
    const std::vector<DrawCommandData>& commands)
{
    if (commands.empty())
        return;

    ssbo.upload(
        commands.data(),
        sizeof(DrawCommandData) * commands.size());
}

void DrawBuffer::bind() const
{
    ssbo.bindBase(Binding);
}

size_t DrawBuffer::size() const
{
    return ssbo.getSize() / sizeof(DrawCommandData);
}


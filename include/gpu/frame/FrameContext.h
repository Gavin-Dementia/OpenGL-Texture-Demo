#ifndef FRAME_CONTEXT_H
#define FRAME_CONTEXT_H
#include <glad/glad.h>

struct FrameContext
{
    uint32_t instanceCount = 0;
    uint32_t visibleCount  = 0;
    uint32_t drawCount     = 0;

    uint32_t dispatchGroups = 0;
};

#endif // FRAME_CONTEXT_H


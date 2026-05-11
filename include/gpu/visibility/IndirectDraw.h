#ifndef INDIRECT_DRAW_H
#define INDIRECT_DRAW_H


struct DrawElementsIndirectCommand
{
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    uint32_t baseVertex;
    uint32_t baseInstance;
};

#endif // INDIRECT_DRAW_H
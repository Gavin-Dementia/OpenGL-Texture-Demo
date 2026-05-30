#ifndef INDIRECT_COMMAND_BUFFER_H
#define INDIRECT_COMMAND_BUFFER_H


struct DrawElementsIndirectCommandBuffer
{
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    uint32_t baseVertex;
    uint32_t baseInstance;
    uint32_t _pad;   // ❗ 强制 16-byte align
};

#endif // INDIRECT_COMMAND_BUFFER_H


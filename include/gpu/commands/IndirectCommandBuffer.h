#ifndef INDIRECT_COMMAND_BUFFER_H
#define INDIRECT_COMMAND_BUFFER_H


struct IndirectCommandBuffer
{
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    uint32_t baseVertex;
    uint32_t baseInstance;
};

#endif // INDIRECT_COMMAND_BUFFER_H


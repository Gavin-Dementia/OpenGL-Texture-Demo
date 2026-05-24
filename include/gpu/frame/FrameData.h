#ifndef FRAMEDATA_H
#define FRAMEDATA_H
#include <glm/glm.hpp>

struct FrameData
{
    glm::mat4 view;
    glm::mat4 projection;

    GLuint objectBuffer;
    GLuint indirectBuffer;

    uint32_t visibleCount;
};

#endif // FRAMEDATA_H


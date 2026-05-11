#ifndef VISIBILITY_SYSTEM_H
#define VISIBILITY_SYSTEM_H
#include <glad/glad.h>

#include "gpu/buffers/SSBO.h"
#include "gpu/scene/GPUScene.h"
#include "graphics/Shader.h"


class VisibilitySystem
{
public:

    void init();

    void dispatchCulling(
        GLuint instanceBuffer,
        GLuint transformBuffer,
        GLuint meshBuffer,
        GLuint cameraUBO,
        int instanceCount);

    void bindBuffers() const;

    GLuint getIndirectBuffer() const { return indirectBuffer.getID(); }
    GLuint getVisibleBuffer() const { return visibleInstanceBuffer.getID(); }

    void runCullCompute(GPUScene&, Shader&) {}
int getDrawCount() const { return 0; }
private:

    SSBO visibleInstanceBuffer;
    SSBO indirectBuffer;
    SSBO counterBuffer;

    GLuint computeProgram = 0;
};

#endif // VISIBILITY_SYSTEM_H


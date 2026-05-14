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
        GPUScene& gpuScene,
        int instanceCount);

    void bindBuffers() const;
    void bindInputs(const GPUScene& scene);
    void bindOutputs() const;

    GLuint getIndirectBuffer() const { return indirectCommandBuffer.getID(); }
    GLuint getVisibleBuffer() const { return visibleInstanceBuffer.getID(); }
    GLuint getCounterBuffer() const { return counterBuffer.getID(); }

    void runCullCompute(GPUScene&, Shader&) {}
    int getDrawCount() const { return 0; }
private:

    SSBO indirectCommandBuffer;
    SSBO visibleInstanceBuffer;
    SSBO counterBuffer;

    Shader computeProgram;
};

#endif // VISIBILITY_SYSTEM_H


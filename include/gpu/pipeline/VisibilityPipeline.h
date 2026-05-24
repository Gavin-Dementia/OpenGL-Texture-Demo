#ifndef VISIBILITY_SYSTEM_H
#define VISIBILITY_SYSTEM_H
#include <glad/glad.h>

#include "gpu/buffers/SSBO.h"
#include "gpu/upload/SceneUploader.h"
#include "graphics/Shader.h"


class VisibilityPipeline
{
public:

    void init();

    void dispatchCulling(
        SceneUploader& scene,
        int instanceCount);

    void bindBuffers() const;
    void bindInputs(const SceneUploader& scene);
    void bindOutputs() const;

    GLuint getIndirectBuffer() const { return indirectCommandBuffer.getID(); }
    GLuint getVisibleBuffer() const { return visibleInstanceBuffer.getID(); }
    GLuint getCounterBuffer() const { return counterBuffer.getID(); }

    void runCullCompute(SceneUploader&, Shader&) {}
    int getDrawCount() const { return 0; }
private:

    SSBO indirectCommandBuffer;
    SSBO visibleInstanceBuffer;
    SSBO counterBuffer;

    Shader computeProgram;
};

#endif // VISIBILITY_SYSTEM_H


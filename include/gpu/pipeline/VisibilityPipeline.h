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
    void bindOutputs(const SceneUploader& scene);

    void dispatch(
        SceneUploader& scene,
        int instanceCount);

    GLuint getVisibleBuffer() const { return VisibilityBuffer.getID(); }
    GLuint getCounterBuffer() const { return CounterBuffer.getID(); }

    void runCullCompute(SceneUploader&, Shader&) {}

private:

    SSBO VisibilityBuffer;
    SSBO CounterBuffer;

    Shader computeProgram;
};

#endif // VISIBILITY_SYSTEM_H


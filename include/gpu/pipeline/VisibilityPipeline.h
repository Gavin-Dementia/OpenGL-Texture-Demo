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

    // void bindBuffers() const;
    // void bindOutputs(const SceneUploader& scene);

    // void dispatch(
    //     SceneUploader& scene,
    //     int instanceCount);
    
    void dispatch(
        const SceneUploader& scene,
        DrawBuffer& drawBuffer,
        GLuint& outDrawCount,
        int instanceCount);

    // GLuint getVisibleBuffer() const { return visibilityBuffer.getID(); }
    GLuint getCounterBuffer() const { return counterBuffer.getID(); }

private:
    void bindInputs(const SceneUploader& scene);
    void bindOutputs(DrawBuffer& drawBuffer);

private:

    // SSBO visibilityBuffer;
    SSBO counterBuffer;

    Shader computeProgram;
};

#endif // VISIBILITY_SYSTEM_H


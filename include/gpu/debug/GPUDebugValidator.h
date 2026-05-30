#ifndef GPUDebugValidator_H
#define GPUDebugValidator_H
#include <glad/glad.h>

class BindingSanityChecker;
class BufferInspector;
class PipelineStateTracker;

class GPUDebugValidator
{
public:
    void init();

    void beginFrame();
    void endFrame();

    void validateComputeDispatch(GLuint expectedInstanceCount);
    void validateIndirectBuffer(GLuint drawBuffer);
    void validateDrawCount(GLuint drawCount);

    void attachChecker(BindingSanityChecker* checker);
    void attachInspector(BufferInspector* inspector);
    void attachTracker(PipelineStateTracker* tracker);

private:
    BindingSanityChecker* checker = nullptr;
    BufferInspector* inspector = nullptr;
    PipelineStateTracker* tracker = nullptr;

    int frameIndex = 0;
};

#endif // GPUDebugValidator_H


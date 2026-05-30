#include "gpu/debug/GPUDebugValidator.h"
#include "gpu/debug/BindingSanityChecker.h"
#include "gpu/debug/BufferInspector.h"
#include "gpu/debug/PipelineStateTracker.h"

#include <iostream>

void GPUDebugValidator::init()
{
    frameIndex = 0;
}

void GPUDebugValidator::attachChecker(BindingSanityChecker* c)
{
    checker = c;
}

void GPUDebugValidator::attachInspector(BufferInspector* i)
{
    inspector = i;
}

void GPUDebugValidator::attachTracker(PipelineStateTracker* t)
{
    tracker = t;
}

void GPUDebugValidator::beginFrame()
{
    frameIndex++;
}

void GPUDebugValidator::endFrame()
{
    std::cout << "[GPU DEBUG] Frame " << frameIndex << " end\n";
}

void GPUDebugValidator::validateComputeDispatch(GLuint expectedInstanceCount)
{
    if (!tracker) return;

    if (tracker->lastDispatchSize == 0)
    {
        std::cerr << "[GPU DEBUG] ERROR: compute not dispatched\n";
    }

    if (tracker->lastDispatchSize < expectedInstanceCount)
    {
        std::cerr << "[GPU DEBUG] WARNING: dispatch smaller than instance count\n";
    }
}

void GPUDebugValidator::validateIndirectBuffer(GLuint drawBuffer)
{
    if (!inspector) return;

    GLuint cmdCount = inspector->peekIndirectCount(drawBuffer);

    if (cmdCount == 0)
    {
        std::cerr << "[GPU DEBUG] ERROR: indirect draw buffer is empty\n";
    }
}

void GPUDebugValidator::validateDrawCount(GLuint drawCount)
{
    if (drawCount == 0)
    {
        std::cerr << "[GPU DEBUG] WARNING: drawCount == 0 (nothing will render)\n";
    }
}


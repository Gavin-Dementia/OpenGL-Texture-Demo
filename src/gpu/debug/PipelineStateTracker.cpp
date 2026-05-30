#include "gpu/debug/PipelineStateTracker.h"

void PipelineStateTracker::reset()
{
    lastDispatchSize = 0;
    lastDrawCount = 0;
}

void PipelineStateTracker::setDispatch(GLuint groups)
{
    lastDispatchSize = groups;
}

void PipelineStateTracker::setDrawCount(GLuint count)
{
    lastDrawCount = count;
}

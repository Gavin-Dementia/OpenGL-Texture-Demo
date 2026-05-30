#ifndef PipelineStateTracker_H
#define PipelineStateTracker_H

#include <glad/glad.h>

class PipelineStateTracker
{
public:
    void reset();

    void setDispatch(GLuint groups);
    void setDrawCount(GLuint count);

    GLuint lastDispatchSize = 0;
    GLuint lastDrawCount = 0;
};

#endif // PipelineStateTracker_H


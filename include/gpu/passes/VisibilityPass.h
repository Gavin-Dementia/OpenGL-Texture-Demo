#ifndef VISIBILITY_PASS_H
#define VISIBILITY_PASS_H

#include <glad/glad.h>
#include "graphics/Shader.h"
#include "gpu/buffers/SSBO.h"  
#include "gpu/frame/FrameContext.h" 

class SceneUploader;
class DrawBuffer;
class CounterBuffer;
class Scene;

class VisibilityPass
{
public:

    void init();

    void execute(FrameContext& frame, Scene& scene);

private:

    Shader computeProgram;
};

#endif // VISIBILITY_PASS_H


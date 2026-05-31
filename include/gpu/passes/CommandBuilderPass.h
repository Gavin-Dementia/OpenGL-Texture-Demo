#ifndef  COMMAND_BUILDER_PASS_H
#define  COMMAND_BUILDER_PASS_H

#include <glad/glad.h>
#include "gpu/frame/FrameContext.h"
#include "graphics/Shader.h"

struct FrameContext;

class CommandBuilderPass
{
public:
    void init();
    void execute(const FrameContext& frame);

private:
    Shader computeProgram;
};

#endif //COMMAND_BUILDER_PASS_H
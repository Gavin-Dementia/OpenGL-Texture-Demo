#include "gpu/passes/CommandBuilderPass.h"
#include "core/ShaderLoader.h"

void CommandBuilderPass::init()
{
    computeProgram = ShaderLoader::loadCompute("command_builder.comp");
}

void CommandBuilderPass::execute(const FrameContext& frame)
{
    computeProgram.use();

    GLuint groups =
        (frame.drawCount + 63) / 64;

    glDispatchCompute(groups, 1, 1);

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT |
        GL_COMMAND_BARRIER_BIT
    );
}
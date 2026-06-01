#include "gpu/backend/RendererGPU.h"
#include "gpu/backend/BindingManager.h"
#include "gpu/upload/SceneUploader.h"
#include "graphics/Shader.h"
#include "core/ShaderLoader.h"
#include "scene/Scene.h"
#include "gpu/frame/FrameContext.h"
#include "gpu/passes/DrawPass.h"
#include "gpu/passes/VisibilityPass.h"
#include "gpu/layout/Binding.h"

#include <iostream>

#include "gpu/commands/DrawElementsIndirectCommandBuffer.h"

void RendererGPU::init()
{
    std::cout << "RendererGPU init\n";
}

void RendererGPU::render(Scene& scene,
                         SceneUploader& uploader,
                         BindingManager& binding,
                         DrawPass& draw,
                         GLuint vao)
{
    debug.beginFrame();

    // =====================================================
    // 1. CPU snapshot
    uint32_t instanceCount =
        uploader.getInstanceBuffer().size();

    // =====================================================
    // 2. reset counter
    uploader.getCounterBuffer().reset();
    uploader.getCounterBuffer().uploadInstanceCount(instanceCount);

    Shader compute= ShaderLoader::loadCompute("cull.comp");
    compute.use();

    // =====================================================
    // 3. bind compute inputs
    binding.bindForCompute(uploader);

    // =====================================================
    // 4. GPU cull
    glDispatchCompute((instanceCount + 63) / 64, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_COMMAND_BARRIER_BIT);

    // =====================================================
    // 5. bind draw
    binding.bindForDraw(uploader);

    draw.bind(vao,
              uploader.getDrawBuffer(),
              uploader.getCounterBuffer());

     {
        GLuint c = 0;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, uploader.getCounterBuffer().getID());
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &c);

        std::cout << "[DEBUG] drawCount = " << c << std::endl;


        struct Cmd {
            int count;
            int instanceCount;
            int firstIndex;
            int baseVertex;
            int baseInstance;
        };

        Cmd cmd{};
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, uploader.getDrawBuffer().getID());
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Cmd), &cmd);
        std::cout << "[DEBUG] cmd.count = " << cmd.count << std::endl;
        std::cout << "[DEBUG] cmd.instanceCount = " << cmd.instanceCount << std::endl;
        std::cout << "[DEBUG] cmd.firstIndex = " << cmd.firstIndex << std::endl;
        std::cout << "[DEBUG] cmd.baseVertex = " << cmd.baseVertex << std::endl;
        std::cout << "[DEBUG] cmd.baseInstance = " << cmd.baseInstance << std::endl;
        GLint vao = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
        std::cout << "[DEBUG] VAO = " << vao << std::endl;
        GLint ebo = 0;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
        std::cout << "[DEBUG] EBO binding = " << ebo << std::endl;
    
    }         

    // =====================================================
    // 6. indirect draw (NO CONDITION)
    draw.execute(MAX_DRAWS);

    debug.endFrame();
}

void RendererGPU::drawIndirect(int drawCount)
{
    std::cout << "[RendererGPU] drawIndirect legacy call\n";

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,
        GL_UNSIGNED_INT,
        nullptr,
        drawCount,
        0
    );
}


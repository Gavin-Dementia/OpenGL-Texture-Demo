#include "gpu/backend/BindingManager.h"
#include "gpu/upload/SceneUploader.h"
#include "gpu/layout/Binding.h"
#include <iostream>

void BindingManager::bindSSBO(GLuint binding, GLuint buffer)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
    bound[binding] = buffer;
}

void BindingManager::bindFrame(const SceneUploader& scene)
{
    bindSSBO(Binding::ObjectBuffer,     scene.getObjectBuffer().getID());
    bindSSBO(Binding::TransformBuffer,  scene.getTransformBuffer().getID());
    bindSSBO(Binding::MeshBoundsBuffer, scene.getMeshBoundsBuffer().getID());
    bindSSBO(Binding::InstanceBuffer,   scene.getInstanceBuffer().getID());
    bindSSBO(Binding::MaterialBuffer,   scene.getMaterialBuffer().getID());

    bindSSBO(Binding::DebugBuffer,      scene.getDebugBuffer().getID());
}

void BindingManager::bindForCompute(const SceneUploader& scene)
{
    bindFrame(scene);

    bindSSBO(Binding::CounterBuffer,    scene.getCounterBuffer().getID());
    bindSSBO(Binding::VisibilityBuffer, scene.getVisibilityBuffer().getID());
    bindSSBO(Binding::DrawBuffer,       scene.getDrawBuffer().getID());
}

void BindingManager::bindForDraw(const SceneUploader& scene)
{
    bindFrame(scene);

    bindSSBO(Binding::VisibilityBuffer,  scene.getVisibilityBuffer().getID());

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,scene.getDrawBuffer().getID());
}

void BindingManager::bindForVisibility(
    const SceneUploader& scene)
{
    bindFrame(scene);

    bindSSBO( Binding::CounterBuffer,    scene.getCounterBuffer().getID());
    bindSSBO( Binding::VisibilityBuffer, scene.getVisibilityBuffer().getID());
}

void BindingManager::bindForCommandBuilder(
    const SceneUploader& scene)
{
    bindSSBO( Binding::CounterBuffer,   scene.getCounterBuffer().getID());
    bindSSBO( Binding::VisibilityBuffer,scene.getVisibilityBuffer().getID());
    bindSSBO( Binding::DrawBuffer,      scene.getDrawBuffer().getID());
}

void BindingManager::reset()
{
    for (int i = 0; i < 12; i++)
        bound[i] = 0;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

bool BindingManager::validateBindings() const
{
    bool ok = true;

    auto check = [&](int b, const char* name)
    {
        if (bound[b] == 0)
        {
            std::cerr << "[BIND ERROR] missing: " << name
                      << " at binding " << b << "\n";
            ok = false;
        }
    };

    check(Binding::ObjectBuffer, "ObjectBuffer");
    check(Binding::TransformBuffer, "TransformBuffer");
    check(Binding::InstanceBuffer, "InstanceBuffer");
    check(Binding::CounterBuffer, "CounterBuffer");
    check(Binding::VisibilityBuffer, "VisibilityBuffer");
    check(Binding::DrawBuffer, "DrawBuffer");

    return ok;
}


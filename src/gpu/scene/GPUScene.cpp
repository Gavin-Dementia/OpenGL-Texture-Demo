#include "gpu/scene/GPUScene.h"

void GPUScene::init()
{
    // binding points
    meshBuffer.create(1024 * 1024, 0);
    materialBuffer.create(1024 * 1024, 1);
    transformBuffer.create(1024 * 1024, 2);
    instanceBuffer.create(1024 * 1024, 3);

    // ======================
    // CAMERA UBO (IMPORTANT)
    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraGPU), nullptr, GL_DYNAMIC_DRAW);

    // binding = 0
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);
}

void GPUScene::uploadMeshes(const std::vector<GPUMesh>& meshes)
{
    meshBuffer.upload(meshes.data(),
                       meshes.size() * sizeof(GPUMesh));
}

void GPUScene::uploadMaterials(const std::vector<GPUMaterial>& materials)
{
    materialBuffer.upload(materials.data(),
                          materials.size() * sizeof(GPUMaterial));
}

void GPUScene::uploadTransforms(const std::vector<GPUTransform>& transforms)
{
    transformBuffer.upload(transforms.data(),
                           transforms.size() * sizeof(GPUTransform));
}

void GPUScene::uploadInstances(const std::vector<GPUInstance>& instances)
{
    instanceBuffer.upload(instances.data(),
                          instances.size() * sizeof(GPUInstance));
}

void GPUScene::updateCamera(const Camera& cam, float aspect)
{
    CameraGPU data;

    data.view = cam.GetViewMatrix();
    data.projection = cam.GetProjectionMatrix(aspect);
    data.position = glm::vec4(cam.Position, 1.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraGPU), &data);
}

void GPUScene::bindAll() const
{
    meshBuffer.bindBase();
    materialBuffer.bindBase();
    transformBuffer.bindBase();
    instanceBuffer.bindBase();
}


#include "gpu/upload/SceneUploader.h"

void SceneUploader::init()
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

void SceneUploader::uploadMeshes(const std::vector<MeshDrawData>& meshes)
{
    meshBuffer.upload(meshes.data(),
                       meshes.size() * sizeof(MeshDrawData));
}

void SceneUploader::uploadMaterials(const std::vector<GPUMaterialData>& materials)
{
    materialBuffer.upload(materials.data(),
                          materials.size() * sizeof(GPUMaterialData));
}

void SceneUploader::uploadTransforms(const std::vector<GPUTransformData>& transforms)
{
    transformBuffer.upload(transforms.data(),
                           transforms.size() * sizeof(GPUTransformData));
}

void SceneUploader::uploadInstances(const std::vector<GPUObjectData>& instances)
{
    instanceBuffer.upload(instances.data(),
                          instances.size() * sizeof(GPUObjectData));
}

void SceneUploader::updateCamera(const Camera& cam, float aspect)
{
    CameraGPU data;

    data.view = cam.GetViewMatrix();
    data.projection = cam.GetProjectionMatrix(aspect);
    data.position = glm::vec4(cam.Position, 1.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraGPU), &data);
}

void SceneUploader::bindAll() const
{
    meshBuffer.bindBase();
    materialBuffer.bindBase();
    transformBuffer.bindBase();
    instanceBuffer.bindBase();
}


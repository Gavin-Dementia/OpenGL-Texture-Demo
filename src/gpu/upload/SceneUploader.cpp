#include "gpu/upload/SceneUploader.h"

void SceneUploader::init()
{
    drawBuffer.init();
    materialBuffer.init();
    transformBuffer.init();
    objectBuffer.init();

    // CAMERA UBO 
    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraGPU), nullptr, GL_DYNAMIC_DRAW);
    
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);// binding = 0
}

void SceneUploader::uploadDrawData(const std::vector<DrawCommandData>& drawdata)
{    drawBuffer.upload(drawdata);  }

void SceneUploader::uploadMaterials(const std::vector<GPUMaterialData>& materials)
{    materialBuffer.upload(materials);  }

void SceneUploader::uploadTransforms(const std::vector<GPUTransformData>& transforms)
{    transformBuffer.upload(transforms);  }

void SceneUploader::uploadObjects(const std::vector<GPUObjectData>& objects)
{    objectBuffer.upload(objects);  }

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
    drawBuffer.bind();
    materialBuffer.bind();
    transformBuffer.bind();
    objectBuffer.bind();
}


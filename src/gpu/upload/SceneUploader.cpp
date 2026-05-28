#include "gpu/upload/SceneUploader.h"
#include "gpu/layout/GPUObjectData.h"
#include "gpu/layout/GPUTransformData.h"
#include "gpu/layout/GPUMaterialData.h"

void SceneUploader::init()
{
    drawBuffer.init();
    materialBuffer.init();
    transformBuffer.init();
    objectBuffer.init();
    meshBoundsBuffer.init();

    // CAMERA UBO 
    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraGPU), nullptr, GL_DYNAMIC_DRAW);
    
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);// binding = 0
}

void SceneUploader::uploadDrawData(const std::vector<MeshDrawData>& drawdata)
{    drawBuffer.upload(drawdata);  }

void SceneUploader::uploadDrawData(const MeshDrawData& data)
{    uploadDrawData(std::vector<MeshDrawData>{data});  }

void SceneUploader::uploadMaterials(const std::vector<GPUMaterialData>& materials)
{    materialBuffer.upload(materials);  }

void SceneUploader::uploadMaterials(const GPUMaterialData& material)
{    uploadMaterials(std::vector<GPUMaterialData>{material});  }

void SceneUploader::uploadMeshBounds(const std::vector<glm::vec4>& bounds)
{    meshBoundsBuffer.upload(bounds);  }

void SceneUploader::uploadMeshBounds(const glm::vec4& bound)
{    uploadMeshBounds(std::vector<glm::vec4>{bound});  }

void SceneUploader::uploadTransforms(const std::vector<GPUTransformData>& transforms)
{    transformBuffer.upload(transforms);  }

void SceneUploader::uploadTransforms(const GPUTransformData& transform)
{    uploadTransforms(std::vector<GPUTransformData>{transform});  }

void SceneUploader::uploadObjects(const std::vector<GPUObjectData>& objects)
{    objectBuffer.upload(objects);  }

void SceneUploader::uploadObjects(const GPUObjectData& obj)
{    uploadObjects(std::vector<GPUObjectData>{obj});  }

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
    // drawBuffer.bind();
    materialBuffer.bind();
    transformBuffer.bind();
    objectBuffer.bind();
    meshBoundsBuffer.bind();
}


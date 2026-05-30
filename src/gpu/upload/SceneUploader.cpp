#include "gpu/upload/SceneUploader.h"
#include "gpu/layout/GPUObjectData.h"
#include "gpu/layout/GPUTransformData.h"
#include "gpu/layout/GPUMaterialData.h"

void SceneUploader::init()
{
    // Core scene buffers
    drawBuffer.init();
    materialBuffer.init();
    meshBoundsBuffer.init();
    meshBuffer.init();
    objectBuffer.init();
    transformBuffer.init();
    // GPU Driven buffers
    counterBuffer.init();
    debugBuffer.init(sizeof(glm::vec4) * 1024);
    instanceBuffer.init(1024);
    visibilityBuffer.init(1024);

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

MeshDrawData SceneUploader::uploadMesh(const Mesh& mesh)
{
    MeshDrawData draw= meshBuffer.uploadMesh(mesh);

    uploadDrawData(draw);

    return draw;
}

void SceneUploader::uploadTransforms(const std::vector<GPUTransformData>& transforms)
{    transformBuffer.upload(transforms);  }

void SceneUploader::uploadTransforms(const GPUTransformData& transform)
{    uploadTransforms(std::vector<GPUTransformData>{transform});  }

void SceneUploader::uploadObjects(const std::vector<GPUObjectData>& objects)
{    
    if (objects.empty())  return;
    // objectBuffer.upload(objects);  
    std::vector<Instance> instances;
    instances.reserve(objects.size());

    for (auto& o : objects)
    {
        Instance i;
        i.transformID = o.transformID;
        i.meshID = o.meshID;
        i.materialID = o.materialID;
        i.visibilityID = o.visibilityID;

        instances.push_back(i);
    }

    instanceBuffer.upload(instances.data(), instances.size());

}

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
    objectBuffer.bind();
    transformBuffer.bind();
    meshBoundsBuffer.bind();
    instanceBuffer.bind();
    materialBuffer.bind();
    drawBuffer.bind();
    counterBuffer.bind();
    visibilityBuffer.bind();
}


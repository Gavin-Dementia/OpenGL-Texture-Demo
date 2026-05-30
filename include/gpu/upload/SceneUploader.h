#ifndef SCENE_UPLOADER_H
#define SCENE_UPLOADER_H
#include <vector>
#include <glm/glm.hpp>

#include "gpu/buffers/DrawBuffer.h"
#include "gpu/buffers/MaterialBuffer.h"
#include "gpu/buffers/MeshBoundsBuffer.h"
#include "gpu/buffers/MeshBuffer.h"
#include "gpu/buffers/ObjectBuffer.h"
#include "gpu/buffers/TransformBuffer.h"
#include "graphics/Camera.h"

#include "scene/Mesh.h"

class SceneUploader
{
public:

    void init();

    template<typename T>
    void uploadSingle(const T& data)
    {
        std::vector<T> v(1);
        v[0] = data;
        upload(v);
    }

    // static upload
    void uploadDrawData(const std::vector<MeshDrawData>& drawdata);
    void uploadDrawData(const MeshDrawData& data);
    void uploadMaterials(const std::vector<GPUMaterialData>& materials);
    void uploadMaterials(const GPUMaterialData& material);
    void uploadMeshBounds(const std::vector<glm::vec4>& bounds);
    void uploadMeshBounds(const glm::vec4& bound);
    MeshDrawData uploadMesh(const Mesh& mesh);

    // uint32_t uploadMesh(const Mesh& mesh);

    // per-frame upload
    void uploadObjects(const std::vector<GPUObjectData>& objects);
    void uploadObjects(const GPUObjectData& obj);
    void uploadTransforms(const std::vector<GPUTransformData>& transforms);
    void uploadTransforms(const GPUTransformData& transform);

    // CAMERA (IMPORTANT)
    void updateCamera(const Camera& cam, float aspect);

    // bind all SSBOs
    void bindAll() const;

    const DrawBuffer& getDrawBuffer() const { return drawBuffer; }
    const MaterialBuffer& getMaterialBuffer() const { return materialBuffer; }
    const MeshBoundsBuffer& getMeshBoundsBuffer() const { return meshBoundsBuffer; }
    const MeshBuffer& getMeshBuffer() const { return meshBuffer; }
    const ObjectBuffer& getObjectBuffer() const { return objectBuffer; }
    const TransformBuffer& getTransformBuffer() const { return transformBuffer; }

    GLuint getCameraUBO() const { return cameraUBO; }
    GLuint getGlobalVAO() const { return 0; }

private:
    struct CameraGPU
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec4 position;
    };

    GLuint cameraUBO = 0;

    DrawBuffer drawBuffer;
    MaterialBuffer materialBuffer;
    MeshBoundsBuffer meshBoundsBuffer;
    MeshBuffer meshBuffer;
    ObjectBuffer objectBuffer;
    TransformBuffer transformBuffer;
};


#endif // SCENE_UPLOADER_H


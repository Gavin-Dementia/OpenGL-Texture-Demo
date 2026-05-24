#ifndef SCENE_UPLOADER_H
#define SCENE_UPLOADER_H
#include <vector>

#include "gpu/buffers/SSBO.h"
#include "gpu/layout/GPUTransformData.h"
#include "gpu/layout/MeshDrawData.h"
#include "gpu/layout/GPUMaterialData.h"
#include "gpu/layout/GPUObjectData.h"
#include "scene/Scene.h"
#include "graphics/Camera.h"

class SceneUploader
{
public:

    void init();

    // static upload
    void uploadMeshes(const std::vector<MeshDrawData>& meshes);
    void uploadMaterials(const std::vector<GPUMaterialData>& materials);

    // per-frame upload
    void uploadTransforms(const std::vector<GPUTransformData>& transforms);
    void uploadInstances(const std::vector<GPUObjectData>& instances);

    // CAMERA (IMPORTANT)
    void updateCamera(const Camera& cam, float aspect);

    // bind all SSBOs
    void bindAll() const;

    const SSBO& getMeshBuffer() const { return meshBuffer; }
    const SSBO& getMaterialBuffer() const { return materialBuffer; }
    const SSBO& getTransformBuffer() const { return transformBuffer; }
    const SSBO& getInstanceBuffer() const { return instanceBuffer; }
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

    SSBO meshBuffer;
    SSBO materialBuffer;
    SSBO transformBuffer;
    SSBO instanceBuffer;
};


#endif // SCENE_UPLOADER_H


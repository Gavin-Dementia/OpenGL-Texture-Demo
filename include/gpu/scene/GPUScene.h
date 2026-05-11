#ifndef GPU_SCENE_H
#define GPU_SCENE_H
#include <vector>

#include "gpu/buffers/SSBO.h"
#include "gpu/scene/GPUTransform.h"
#include "gpu/scene/GPUMesh.h"
#include "gpu/scene/GPUMaterial.h"
#include "gpu/scene/GPUInstance.h"
#include "scene/Scene.h"
#include "graphics/Camera.h"

class GPUScene
{
public:

    void init();

    // static upload
    void uploadMeshes(const std::vector<GPUMesh>& meshes);
    void uploadMaterials(const std::vector<GPUMaterial>& materials);

    // per-frame upload
    void uploadTransforms(const std::vector<GPUTransform>& transforms);
    void uploadInstances(const std::vector<GPUInstance>& instances);

    // CAMERA (IMPORTANT)
    void updateCamera(const Camera& cam, float aspect);

    // bind all SSBOs
    void bindAll() const;

    SSBO& getMeshBuffer() { return meshBuffer; }
    SSBO& getMaterialBuffer() { return materialBuffer; }
    SSBO& getTransformBuffer() { return transformBuffer; }
    SSBO& getInstanceBuffer() { return instanceBuffer; }

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


#endif // GPU_SCENE_H


#ifndef SCENE_UPLOADER_H
#define SCENE_UPLOADER_H
#include <vector>

#include "gpu/buffers/SSBO.h"
#include "gpu/layout/MeshDrawData.h"
#include "gpu/layout/GPUTransformData.h"
#include "gpu/layout/GPUMaterialData.h"
#include "gpu/layout/GPUObjectData.h"
#include "scene/Scene.h"
#include "graphics/Camera.h"

#include "gpu/buffers/ObjectBuffer.h"
#include "gpu/buffers/MaterialBuffer.h"
#include "gpu/buffers/TransformBuffer.h"
#include "gpu/buffers/DrawBuffer.h"

class SceneUploader
{
public:

    void init();

    // static upload
    void uploadDrawData(const std::vector<MeshDrawData>& drawdata);
    void uploadMaterials(const std::vector<GPUMaterialData>& materials);

    // per-frame upload
    void uploadTransforms(const std::vector<GPUTransformData>& transforms);
    void uploadObjects(const std::vector<GPUObjectData>& objects);

    // CAMERA (IMPORTANT)
    void updateCamera(const Camera& cam, float aspect);

    // bind all SSBOs
    void bindAll() const;

    const ObjectBuffer& getObjectBuffer() const { return objectBuffer; }
    const MaterialBuffer& getMaterialBuffer() const { return materialBuffer; }
    const TransformBuffer& getTransformBuffer() const { return transformBuffer; }
    const DrawBuffer& getDrawBuffer() const { return drawBuffer; }

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
    TransformBuffer transformBuffer;
    ObjectBuffer objectBuffer;
};


#endif // SCENE_UPLOADER_H


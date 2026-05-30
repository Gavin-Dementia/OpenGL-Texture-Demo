#ifndef SCENE_UPLOADER_H
#define SCENE_UPLOADER_H
#include <vector>
#include <glm/glm.hpp>

#include "gpu/buffers/CounterBuffer.h"
#include "gpu/buffers/DebugBuffer.h"
#include "gpu/buffers/DrawBuffer.h"
#include "gpu/buffers/InstanceBuffer.h"
#include "gpu/buffers/MaterialBuffer.h"
#include "gpu/buffers/MeshBoundsBuffer.h"
#include "gpu/buffers/MeshBuffer.h"
#include "gpu/buffers/ObjectBuffer.h"
#include "gpu/buffers/TransformBuffer.h"
#include "gpu/buffers/VisibilityBuffer.h"
#include "graphics/Camera.h"

#include "scene/Mesh.h"

class SceneUploader
{
public:
    SceneUploader() = default;
    ~SceneUploader() = default;

    SceneUploader(const SceneUploader&) = delete;
    SceneUploader& operator=(const SceneUploader&) = delete;

    void init();

    template<typename T>
    void uploadSingle(const T& data)
    {
        std::vector<T> v(1);
        v[0] = data;
        upload(v);
    }

    // =========================
    // Draw / Mesh
    void uploadDrawData(const std::vector<MeshDrawData>& drawdata);
    void uploadDrawData(const MeshDrawData& data);

    // =========================
    // Material
    void uploadMaterials(const std::vector<GPUMaterialData>& materials);
    void uploadMaterials(const GPUMaterialData& material);

    // =========================
    // Mesh bounds
    void uploadMeshBounds(const std::vector<glm::vec4>& bounds);
    void uploadMeshBounds(const glm::vec4& bound);

    // =========================
    // Mesh upload
    MeshDrawData uploadMesh(const Mesh& mesh);

    // =========================
    // Per-frame data
    void uploadObjects(const std::vector<GPUObjectData>& objects);
    void uploadObjects(const GPUObjectData& obj);

    void uploadTransforms(const std::vector<GPUTransformData>& transforms);
    void uploadTransforms(const GPUTransformData& transform);

    // CAMERA (IMPORTANT)
    void updateCamera(const Camera& cam, float aspect);

    // bind all SSBOs
    void bindAll() const;

    // =========================
    // Buffers
    CounterBuffer& getCounterBuffer(){  return counterBuffer;  };
    const CounterBuffer& getCounterBuffer() const{  return counterBuffer;  };

    DebugBuffer& getDebugBuffer(){  return debugBuffer;  };
    const DebugBuffer& getDebugBuffer() const{  return debugBuffer;  };

    DrawBuffer& getDrawBuffer(){  return drawBuffer;  };
    const DrawBuffer& getDrawBuffer() const{  return drawBuffer;  };

    InstanceBuffer& getInstanceBuffer(){  return instanceBuffer;  };
    const InstanceBuffer& getInstanceBuffer() const{  return instanceBuffer;  };

    MaterialBuffer& getMaterialBuffer(){  return materialBuffer;  };
    const MaterialBuffer& getMaterialBuffer() const{  return materialBuffer;  };

    MeshBoundsBuffer& getMeshBoundsBuffer(){  return meshBoundsBuffer;  };
    const MeshBoundsBuffer& getMeshBoundsBuffer() const{  return meshBoundsBuffer;  };

    MeshBuffer& getMeshBuffer(){  return meshBuffer;  };
    const MeshBuffer& getMeshBuffer() const{  return meshBuffer;  };

    ObjectBuffer& getObjectBuffer(){  return objectBuffer;  };
    const ObjectBuffer& getObjectBuffer() const{  return objectBuffer;  };

    TransformBuffer& getTransformBuffer(){  return transformBuffer;  };
    const TransformBuffer& getTransformBuffer() const{  return transformBuffer;  };

    VisibilityBuffer& getVisibilityBuffer(){  return visibilityBuffer;  };
    const VisibilityBuffer& getVisibilityBuffer() const{  return visibilityBuffer;  };

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

    CounterBuffer counterBuffer;
    DebugBuffer debugBuffer;
    DrawBuffer drawBuffer;
    InstanceBuffer instanceBuffer;
    MaterialBuffer materialBuffer;
    MeshBoundsBuffer meshBoundsBuffer;
    MeshBuffer meshBuffer;
    ObjectBuffer objectBuffer;
    TransformBuffer transformBuffer;
    VisibilityBuffer visibilityBuffer;
};


#endif // SCENE_UPLOADER_H


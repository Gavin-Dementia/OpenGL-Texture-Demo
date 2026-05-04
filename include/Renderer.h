#ifndef RENDERER_H
#define RENDERER_H
#include <unordered_map>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"

class Renderer
{
public:
    struct RenderItem {
        Mesh* mesh = nullptr;
        Material* material = nullptr;
        glm::mat4 model = glm::mat4(1.0f);
        bool isEmissive = false;
        glm::vec3 emissiveColor = glm::vec3(0.0f);
    };

    struct GpuRenderItem 
    {
        glm::mat4 model;
        glm::vec4 bounding; // xyz = center, w = radius
        glm::vec4 emissiveColor;
        uint32_t meshID;
        uint32_t materialID;
        uint32_t emissive;
        uint32_t pad; // std430 alignment
    };

    // Helper struct matching GLSL DrawCommand layout for indirect draws
    struct DrawElementsIndirectCommand 
    {
        uint32_t count;
        uint32_t instanceCount;
        uint32_t firstIndex;
        uint32_t baseVertex;
        uint32_t baseInstance;
    };
    using DrawCommand = DrawElementsIndirectCommand;

    struct GpuMesh 
    {
        uint32_t indexCount;
        uint32_t firstIndex;
        uint32_t baseVertex;
        uint32_t pad;
    };

public:
    void init();
    void init_GPU();
    void render(Scene& scene,
                Shader& shader,
                Shader& lightShader,
                Shader& depthShader,
                Shader& computeShader, 
                Camera& camera,
                float width_, float height_);

    // Public helpers used by render passes
    void executeDrawObjects(Scene& scene, Shader& shader) { drawObjects(scene, shader); }
    void initGlobalVAO();
    GLuint getDepthMapFBO() const { return depthMapFBO; }
    GLuint getDepthMap() const { return depthMap; }
    unsigned int getShadowWidth() const { return SHADOW_WIDTH; }
    unsigned int getShadowHeight() const { return SHADOW_HEIGHT; }

    // wrappers for private upload functions so passes can call them
    void uploadCameraPublic(Shader& shader, Camera& camera, float width, float height) { uploadCamera(shader, camera, width, height); }
    void uploadLightsPublic(Scene& scene) { uploadLights(scene); }

private:
    void bindGpuDrivenBuffersForCompute();
    void copyCounterToIndirectBuffer();
    bool validateCommandBufferCapacity(size_t maxCommandsExpected);
    void dispatchCullingComputeAndDraw(Shader& computeShader, Shader& shader,
                                        Camera& camera, size_t numItems,
                                        GLuint currentVAO);
    void uploadRenderQueue(const std::vector<Renderer::GpuRenderItem>& items);
    std::vector<Renderer::GpuRenderItem> buildRenderQueue(Scene& scene);
    void buildGlobalMeshBuffer(Scene& scene);
    void uploadCamera(Shader& shader, Camera& camera, float width, float height);
    void uploadLights(Scene& scene);
    void drawObjects(Scene& scene, Shader& shader);
    void drawLightObjects(Scene& scene, Shader& lightShader, Camera& camera,
                          float width, float height);
    void drawObjectsGPU(Scene& scene, Shader& shader);
    void drawObjectsGPU(Scene& scene, Shader& shader,
                        Shader& computeShader, Camera& camera,
                        float width_, float height_);

private:
    std::unordered_map<Mesh*, uint32_t> meshIndexMap;
    std::vector<GpuMesh> gpuMeshes;

    GLuint ssboRenderQueue = 0;
    GLuint ssboMeshData = 0;
    GLuint counterBuffer;
    GLuint commandBuffer;

    GLuint globalVAO = 0;
    GLuint globalVBO = 0;
    GLuint globalEBO = 0;    
    
    bool globalMeshBuilt = false;
private:
    GLuint uboDirLight = 0;
    GLuint uboCamera = 0;
    GLuint uboPointLights = 0;
    GLuint uboSpotLight = 0;
    // Shadow map resources
    GLuint depthMapFBO = 0;
    GLuint depthMap = 0;
    const unsigned int SHADOW_WIDTH = 1024;
    const unsigned int SHADOW_HEIGHT = 1024;
    static constexpr uint32_t MAX_COMMANDS = 100000;
    static constexpr uint32_t MAX_RENDER_ITEMS = 100000;
    static constexpr uint32_t MAX_MESHES = 10000;
    static constexpr uint32_t MAX_VERTICES = 10'000'000;
    static constexpr uint32_t MAX_INDICES  = 30'000'000;
};

#endif


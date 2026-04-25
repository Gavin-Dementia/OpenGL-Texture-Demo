#ifndef RENDERER_H
#define RENDERER_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"

class Renderer
{
public:
    void init();
    void render(Scene& scene,
                Shader& shader,
                Shader& lightShader,
                Shader& depthShader,
                Camera& camera,
                float width_, float height_);

    // Public helpers used by render passes
    void executeDrawObjects(Scene& scene, Shader& shader) { drawObjects(scene, shader); }
    GLuint getDepthMapFBO() const { return depthMapFBO; }
    GLuint getDepthMap() const { return depthMap; }
    unsigned int getShadowWidth() const { return SHADOW_WIDTH; }
    unsigned int getShadowHeight() const { return SHADOW_HEIGHT; }

    // wrappers for private upload functions so passes can call them
    void uploadCameraPublic(Shader& shader, Camera& camera, float width, float height) { uploadCamera(shader, camera, width, height); }
    void uploadLightsPublic(Scene& scene) { uploadLights(scene); }

private:
    void uploadCamera(Shader& shader, Camera& camera, float width, float height);
    void uploadLights(Scene& scene);
    void drawObjects(Scene& scene, Shader& shader);
    void drawLightObjects(Scene& scene, Shader& lightShader, Camera& camera,
                          float width, float height);
    void drawObjectsGPU(Scene& scene, Shader& shader);
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
        uint32_t meshID;
        uint32_t materialID;
        uint32_t emissive;
        uint32_t pad; // std430 alignment
        glm::vec4 emissiveColor;
    };
private:
    GLuint ssboRenderQueue = 0;
    GLuint ssboIndirect = 0;
    GLuint ssboInstance = 0;
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
};

#endif


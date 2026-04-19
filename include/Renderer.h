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
                Camera& camera,
                float width_, float height_);

private:
    void uploadCamera(Shader& shader, Camera& camera, float width, float height);
    void uploadLights(Shader& shader, Scene& scene);
    void drawObjects(Scene& scene, Shader& shader);
    void drawLightObjects(Scene& scene, Shader& lightShader, Camera& camera,
                          float width, float height);
private:
    GLuint uboDirLight = 0;
    GLuint uboCamera = 0;
};

#endif


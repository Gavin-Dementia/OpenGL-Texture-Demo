#include "Renderer.h"

void Renderer::render(Scene& scene, Shader& shader,
                      Shader& lightShader, Camera& camera, 
                      float width_, float height_)
{
    // =======================
    // Clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // =======================
    // Camera
    uploadCamera(shader, camera, width_, height_);

    // =======================
    // Lights
    uploadLights(shader, scene);

    // =======================
    // Draw Objects
    drawObjects(scene, shader);

    // =======================
    // Draw Light Objects (for visualization)
    drawLightObjects(scene, lightShader, camera);
}

void Renderer::uploadCamera(Shader& shader, Camera& camera,
                            float width, float height)
{
    glm::mat4 viewMat = camera.GetViewMatrix();
    glm::mat4 projMat = glm::perspective(
        glm::radians(camera.Zoom),
        width / height,
        0.1f,
        100.0f
    );

    shader.use();
    shader.setMat4("viewMat", viewMat);
    shader.setMat4("projMat", projMat);
    shader.setVec3("viewPos", camera.Position);
}

void Renderer::uploadLights(Shader& shader, Scene& scene)
{
    auto& lights = scene.lights;
    // DirLight
    shader.setVec3("dirLight.direction", lights.dirLight.direction);
    shader.setVec3("dirLight.ambient", lights.dirLight.ambient);
    shader.setVec3("dirLight.diffuse", lights.dirLight.diffuse);
    shader.setVec3("dirLight.specular", lights.dirLight.specular);
    // PointLights
    for (int i = 0; i < lights.pointLights.size(); i++)
    {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        auto& l = lights.pointLights[i];

        shader.setVec3(base + ".position", l.position);
        shader.setFloat(base + ".constant", l.constant);
        shader.setFloat(base + ".linear", l.linear);
        shader.setFloat(base + ".quadratic", l.quadratic);
        shader.setVec3(base + ".ambient", l.ambient);
        shader.setVec3(base + ".diffuse", l.diffuse);
        shader.setVec3(base + ".specular", l.specular);
    }
    // SpotLight
    shader.setVec3("spotLight.position", lights.spotLight.position);
    shader.setVec3("spotLight.direction", lights.spotLight.direction);
}

void Renderer::drawObjects(Scene& scene, Shader& shader)
{
    shader.use();

    for (auto& obj : scene.objects)
    {
        shader.setMat4("modelMat", obj.model);
        obj.mesh->draw();
    }
}

void Renderer::drawLightObjects(Scene& scene,
                                Shader& lightShader,
                                Camera& camera)
{
    lightShader.use();

    // camera matrices should still be set
    glm::mat4 viewMat = camera.GetViewMatrix();
    glm::mat4 projMat = glm::perspective(
        glm::radians(camera.Zoom),
        1.0f,  // or pass width/height if needed
        0.1f,
        100.0f
    );

    lightShader.setMat4("viewMat", viewMat);
    lightShader.setMat4("projMat", projMat);

    for (auto& obj : scene.lightObjects)
    {
        lightShader.setMat4("modelMat", obj.model);
        obj.mesh->draw();
    }
}


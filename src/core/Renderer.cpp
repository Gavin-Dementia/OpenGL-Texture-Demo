#include "Renderer.h"

void Renderer::render(Scene& scene, Shader& shader,
                      Shader& lightShader, Camera& camera,
                      float width_, float height_)
{
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uploadCamera(shader, camera, width_, height_);
    uploadLights(shader, scene);

    drawObjects(scene, shader);
    drawLightObjects(scene, lightShader, camera, width_, height_);
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
    shader.use();
    auto& lights = scene.lights;

    shader.setVec3("dirLight.direction", lights.dirLight.direction);
    shader.setVec3("dirLight.ambient", lights.dirLight.ambient);
    shader.setVec3("dirLight.diffuse", lights.dirLight.diffuse);
    shader.setVec3("dirLight.specular", lights.dirLight.specular);

    shader.setInt("numPointLights", (int)lights.pointLights.size());

    for (int i = 0; i < lights.pointLights.size(); i++)
    {
        const auto& l = lights.pointLights[i];
        std::string base = "pointLights[" + std::to_string(i) + "]";

        shader.setVec3(base + ".position", l.position);
        shader.setFloat(base + ".constant", l.constant);
        shader.setFloat(base + ".linear", l.linear);
        shader.setFloat(base + ".quadratic", l.quadratic);
        shader.setVec3(base + ".ambient", l.ambient);
        shader.setVec3(base + ".diffuse", l.diffuse);
        shader.setVec3(base + ".specular", l.specular);
    }

    auto& s = lights.spotLight;

    shader.setVec3("spotLight.position", s.position);
    shader.setVec3("spotLight.direction", s.direction);
    shader.setFloat("spotLight.cutOff", s.cutOff);
    shader.setFloat("spotLight.outerCutOff", s.outerCutOff);

    shader.setFloat("spotLight.constant", s.constant);
    shader.setFloat("spotLight.linear", s.linear);
    shader.setFloat("spotLight.quadratic", s.quadratic);

    shader.setVec3("spotLight.ambient", s.ambient);
    shader.setVec3("spotLight.diffuse", s.diffuse);
    shader.setVec3("spotLight.specular", s.specular);
}

void Renderer::drawObjects(Scene& scene, Shader& shader)
{
    shader.use();

    for (auto& group : scene.renderGroups)
    {
        if (group.models.empty())
            continue;

        // upload instance buffer only when dirty
        if (group.instanceDirty)
        {
            group.mesh->setInstances(group.models);
            group.instanceDirty = false;
        }

        shader.setBool("isEmissive", group.isEmissive);
        shader.setVec3("emissiveColor", group.emissiveColor);

        group.mesh->drawInstanced((int)group.models.size());
    }
}

void Renderer::drawLightObjects(Scene& scene,
                                Shader& lightShader,
                                Camera& camera,
                                float width, float height)
{
    lightShader.use();

    glm::mat4 viewMat = camera.GetViewMatrix();
    glm::mat4 projMat = glm::perspective(
        glm::radians(camera.Zoom),
        width / height,
        0.1f,
        100.0f
    );

    lightShader.setMat4("viewMat", viewMat);
    lightShader.setMat4("projMat", projMat);

    for (auto& obj : scene.lightVisuals)
    {
        lightShader.setMat4("modelLight", obj.getModel());
        obj.mesh->draw();
    }
}


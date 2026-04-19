#include "Renderer.h"

void Renderer::init()
{
    glGenBuffers(1, &uboDirLight);

    glBindBuffer(GL_UNIFORM_BUFFER, uboDirLight);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirLightGPU), nullptr, GL_DYNAMIC_DRAW);

    // binding = 1（要和 GLSL 一致）
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboDirLight, 0, sizeof(DirLightGPU));

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Camera UBO (binding = 0, matches shaders)
    glGenBuffers(1, &uboCamera);
    glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
    // layout: mat4 view, mat4 projection, vec4 viewPos => ensure size
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2 + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboCamera, 0, sizeof(glm::mat4) * 2 + sizeof(glm::vec4));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::render(Scene& scene, Shader& shader,
                      Shader& lightShader, Camera& camera,
                      float width_, float height_)
{
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uploadCamera(shader, camera, width_, height_);

    uploadLights(shader, scene);// 🔥 不再傳 shader

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

    // pack into std140-compatible layout and upload to camera UBO (binding = 0)
    struct CameraGPU {
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec4 viewPos;
    } cam;

    cam.view = viewMat;
    cam.projection = projMat;
    cam.viewPos = glm::vec4(camera.Position, 0.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraGPU), &cam);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::uploadLights(Shader& shader, Scene& scene)
{
    auto& lights = scene.lights;
    
    // =========================
    // 1️⃣ DirLight → UBO（核心）
    // =========================
    glBindBuffer(GL_UNIFORM_BUFFER, uboDirLight);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirLightGPU), &lights.dirLightGPU);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // shader.setVec3("dirLight.direction", lights.dirLight.direction);
    // shader.setVec3("dirLight.ambient", lights.dirLight.ambient);
    // shader.setVec3("dirLight.diffuse", lights.dirLight.diffuse);
    // shader.setVec3("dirLight.specular", lights.dirLight.specular);

    shader.use();
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

    // Camera UBO already uploaded in uploadCamera(); light shader reads from UBO

    for (auto& obj : scene.lightVisuals)
    {
        lightShader.setMat4("modelLight", obj.getModel());
        obj.mesh->draw();
    }
}


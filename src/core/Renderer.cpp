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

    // Point lights UBO (binding = 2)
    const int MAX_POINT_LIGHTS = 16; // must match shader NR_POINT_LIGHTS
    glGenBuffers(1, &uboPointLights);
        glBindBuffer(GL_UNIFORM_BUFFER, uboPointLights);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightGPU) * MAX_POINT_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboPointLights, 0, sizeof(PointLightGPU) * MAX_POINT_LIGHTS);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Spot light UBO (binding = 3)
    glGenBuffers(1, &uboSpotLight);
    glBindBuffer(GL_UNIFORM_BUFFER, uboSpotLight);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLightGPU), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 3, uboSpotLight, 0, sizeof(SpotLightGPU));
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

    // Debug: print current light info to console (helps verify UBO data)
    std::cout << "[Renderer] DirLight direction: "
              << lights.dirLight.direction.x << ", "
              << lights.dirLight.direction.y << ", "
              << lights.dirLight.direction.z << std::endl;
    std::cout << "[Renderer] numPointLights: " << lights.pointLights.size() << std::endl;
    // Fill GPU cache for point lights and upload to UBO (binding = 2)
    const int MAX_POINT_LIGHTS = 16;
    if (lights.pointLightsGPU.size() < MAX_POINT_LIGHTS)
            lights.pointLightsGPU.resize(MAX_POINT_LIGHTS);

    for (size_t i = 0; i < lights.pointLights.size() && i < MAX_POINT_LIGHTS; ++i)
    {
        const auto& l = lights.pointLights[i];
            PointLightGPU pg;
        pg.position = glm::vec4(l.position, 0.0f);
        pg.ambient = glm::vec4(l.ambient, 0.0f);
        pg.diffuse = glm::vec4(l.diffuse, 0.0f);
        pg.specular = glm::vec4(l.specular, 0.0f);
        pg.params = glm::vec4(l.constant, l.linear, l.quadratic, l.intensity);
        lights.pointLightsGPU[i] = pg;
    }

        glBindBuffer(GL_UNIFORM_BUFFER, uboPointLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightGPU) * MAX_POINT_LIGHTS, lights.pointLightsGPU.data());
        // Read back first point light from UBO for verification
        PointLightGPU readBackPL;
        glGetBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightGPU), &readBackPL);
        std::cout << "[Renderer UBO readback] first point pos: "
                  << readBackPL.position.x << ", "
                  << readBackPL.position.y << ", "
                  << readBackPL.position.z << std::endl;
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

    auto& s = lights.spotLight;
    // Fill spotLight GPU struct and upload to UBO (binding = 3)
        SpotLightGPU sg;
    sg.position = glm::vec4(s.position, 0.0f);
    sg.direction = glm::vec4(s.direction, 0.0f);
    sg.ambient = glm::vec4(s.ambient, 0.0f);
    sg.diffuse = glm::vec4(s.diffuse, 0.0f);
    sg.specular = glm::vec4(s.specular, 0.0f);
    sg.params1 = glm::vec4(s.cutOff, s.outerCutOff, s.constant, s.linear);
    sg.params2 = glm::vec4(s.quadratic, s.intensity, 0.0f, 0.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, uboSpotLight);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SpotLightGPU), &sg);
    // read back spot light ubo
        SpotLightGPU readBackSg;
        glGetBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SpotLightGPU), &readBackSg);
        std::cout << "[Renderer UBO readback] spot pos: "
                  << readBackSg.position.x << ", "
                  << readBackSg.position.y << ", "
                  << readBackSg.position.z << std::endl;
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
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


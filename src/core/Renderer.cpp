#include "Renderer.h"
#include "Material.h"
#include <algorithm>
#include <cstdint>

void Renderer::init()
{
    // =========================
    // UBO setup
    // =========================
    glGenBuffers(1, &uboDirLight);
    glBindBuffer(GL_UNIFORM_BUFFER, uboDirLight);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirLightGPU), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboDirLight, 0, sizeof(DirLightGPU));

    glGenBuffers(1, &uboCamera);
    glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2 + sizeof(glm::vec4),
        nullptr, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboCamera, 0,
        sizeof(glm::mat4) * 2 + sizeof(glm::vec4));

    glGenBuffers(1, &uboPointLights);
    glBindBuffer(GL_UNIFORM_BUFFER, uboPointLights);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightGPU) * 16,
        nullptr, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboPointLights, 0,
        sizeof(PointLightGPU) * 16);

    glGenBuffers(1, &uboSpotLight);
    glBindBuffer(GL_UNIFORM_BUFFER, uboSpotLight);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotLightGPU), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 3, uboSpotLight, 0, sizeof(SpotLightGPU));

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // =========================
    // SSBO: Render Queue
    // =========================
    glGenBuffers(1, &ssboRenderQueue);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboRenderQueue);

    // allocate small first（避免 size=0 undefined behavior）
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, nullptr, GL_DYNAMIC_DRAW);

    // binding = 8（要跟 compute shader 一致）
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, ssboRenderQueue);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // =========================
    // Shadow map
    // =========================
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1,1,1,1};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::render(Scene& scene, Shader& shader,
                      Shader& lightShader, Shader& depthShader,
                      Camera& camera,
                      float width_, float height_)
{
    glEnable(GL_DEPTH_TEST);

    // 1. Render scene to depth map (from directional light POV)
    // compute light-space matrix for directional light
    glm::vec3 lightDir = glm::normalize(scene.lights.dirLight.direction);
    float near_plane = 1.0f, far_plane = 50.0f;
    float orthoSize = 15.0f;
    glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
    glm::vec3 lightPos = -lightDir * 20.0f;
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    depthShader.use();
    depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    // render depth-only pass
    drawObjects(scene, depthShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. Render scene normally
    glViewport(0, 0, (int)width_, (int)height_);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uploadCamera(shader, camera, width_, height_);
    uploadLights(scene);

    // bind depth map to texture unit 2
    shader.use();
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    shader.setInt("shadowMap", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    const bool useGPU = true;
    if (useGPU)//GPU path（upload + fallback）
        drawObjectsGPU(scene, shader);
    else//CPU fallback（stable baseline）
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


void Renderer::uploadLights(Scene& scene)
{
    auto& lights = scene.lights;
    // =========================================================
    // 1. Directional Light (UBO 1)
    // =========================================================
    if (lights.dirDirty)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, uboDirLight);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirLightGPU), &lights.dirLightGPU);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        lights.dirDirty = false;
    }

    // =========================================================
    // 2. Point Lights (UBO 2) - fixed array upload
    // =========================================================
    const int MAX_POINT_LIGHTS = 16;
    PointLightGPU gpuPoints[MAX_POINT_LIGHTS] = {};
    int count = std::min((int)lights.pointLights.size(), MAX_POINT_LIGHTS);

    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        if (i < lights.pointLights.size())
        {
            const auto& l = lights.pointLights[i];

            gpuPoints[i].position = glm::vec4(l.position, 0);
            gpuPoints[i].ambient  = glm::vec4(l.ambient, 0);
            gpuPoints[i].diffuse  = glm::vec4(l.diffuse, 0);
            gpuPoints[i].specular = glm::vec4(l.specular, 0);
            gpuPoints[i].params   = glm::vec4(l.constant, l.linear, l.quadratic, 1.0f);
        }
        else
        {   // gpuPoints[i].params = glm::vec4(0,0,0,0);
        }
    }

    glBindBuffer(GL_UNIFORM_BUFFER, uboPointLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLightGPU) * MAX_POINT_LIGHTS, gpuPoints);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // =========================================================
    // 3. Spot Light (UBO 3)
    // =========================================================
    if (lights.spotDirty)
    {
        const auto& s = lights.spotLight;

        SpotLightGPU sg;
        sg.position  = glm::vec4(s.position, 0.0f);
        sg.direction = glm::vec4(s.direction, 0.0f);
        sg.ambient   = glm::vec4(s.ambient, 0.0f);
        sg.diffuse   = glm::vec4(s.diffuse, 0.0f);
        sg.specular  = glm::vec4(s.specular, 0.0f);
        sg.params1   = glm::vec4(s.cutOff, s.outerCutOff, s.constant, s.linear);
        sg.params2   = glm::vec4(s.quadratic, s.intensity, 0.0f, 0.0f);

        glBindBuffer(GL_UNIFORM_BUFFER, uboSpotLight);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SpotLightGPU), &sg);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        lights.spotDirty = false;
    }
}

void Renderer::drawObjects(Scene& scene, Shader& shader)
{
    shader.use();

    // Build render queue: one RenderItem per model instance
    std::vector<RenderItem> queue;
    for (auto& group : scene.renderGroups)
    {
        if (group.models.empty()) continue;

        for (const auto& m : group.models)
        {
            RenderItem it;
            it.mesh = group.mesh;
            it.material = group.material;
            it.model = m;
            it.isEmissive = group.isEmissive;
            it.emissiveColor = group.emissiveColor;
            queue.push_back(it);
        }
    }

    if (queue.empty()) return;

    // Sort by material pointer then mesh pointer to minimize state changes
    std::sort(queue.begin(), queue.end(), [](const RenderItem& a, const RenderItem& b){
        if (a.material != b.material) return a.material < b.material;
        if (a.mesh != b.mesh) return a.mesh < b.mesh;
        return false;
    });

    // Batch contiguous items with same material+mesh using instancing
    RenderItem const* prev = &queue[0];
    std::vector<glm::mat4> batchModels;
    batchModels.reserve(64);

    auto flushBatch = [&](RenderItem const* key){
        if (batchModels.empty()) return;
        // bind material/state
        if (key->material)
            key->material->bind(shader);
        shader.setBool("isEmissive", key->isEmissive);
        shader.setVec3("emissiveColor", key->emissiveColor);
        // upload instances and draw
        key->mesh->setInstances(batchModels);
        key->mesh->drawInstanced((int)batchModels.size());
        batchModels.clear();
    };

    for (size_t i = 0; i < queue.size(); ++i)
    {
        RenderItem const* cur = &queue[i];
        bool sameMaterial = (cur->material == prev->material);
        bool sameMesh = (cur->mesh == prev->mesh);

        if (!sameMaterial || !sameMesh)
        {
            // flush previous batch
            flushBatch(prev);
            prev = cur;
        }

        batchModels.push_back(cur->model);
    }

    // flush remaining
    flushBatch(prev);
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

void Renderer::drawObjectsGPU(Scene& scene, Shader& shader)
{
    shader.use();

    // Build CPU queue
    std::vector<RenderItem> queue;

    for (auto& group : scene.renderGroups)
    {
        if (group.models.empty()) continue;

        for (const auto& m : group.models)
        {
            RenderItem it;
            it.mesh = group.mesh;
            it.material = group.material;
            it.model = m;
            it.isEmissive = group.isEmissive;
            it.emissiveColor = group.emissiveColor;
            queue.push_back(it);
        }
    }
    if (queue.empty()) return;

    // Convert → GPU format
    std::vector<GpuRenderItem> gpuItems;
    gpuItems.reserve(queue.size());

    for (auto& item : queue)
    {
        GpuRenderItem g{};
        g.model = item.model;
        g.meshID = 0;
        g.materialID = 0;
        g.emissive = item.isEmissive ? 1 : 0;
        g.pad = 0;
        g.emissiveColor = glm::vec4(item.emissiveColor, 0.0f);

        gpuItems.push_back(g);
    }

    // Upload to SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboRenderQueue);

    glBufferData(GL_SHADER_STORAGE_BUFFER, gpuItems.size() * sizeof(GpuRenderItem),
        gpuItems.data(), GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, ssboRenderQueue);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // TEMP fallback
    drawObjects(scene, shader);
}


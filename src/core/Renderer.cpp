#include "Renderer.h"
#include "Material.h"
#include <algorithm>
#include <cstdint>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#define BIND_RENDER_QUEUE 8
#define BIND_COUNTER      9
#define BIND_MESH        10
#define BIND_COMMAND     11

// Bind SSBOs / indirect buffer to the bindings expected by the compute shader
void Renderer::bindGpuDrivenBuffersForCompute()
{
    // RenderQueue
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BIND_RENDER_QUEUE, ssboRenderQueue);
    // Counter
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BIND_COUNTER, counterBuffer);
    // Mesh data
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BIND_MESH, ssboMeshData);
    // Command buffer (also bind as GL_DRAW_INDIRECT_BUFFER)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BIND_COMMAND, commandBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
}

void Renderer::initGlobalVAO()
{
    glGenVertexArrays(1, &globalVAO);
    glGenBuffers(1, &globalVBO);
    glGenBuffers(1, &globalEBO);

    glBindVertexArray(globalVAO);

    glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);

    //pos(3) normal(3) uv(2)

    GLsizei stride = sizeof(float) * 8;

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*3));

    glEnableVertexAttribArray(2); // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*6));

    glBindVertexArray(0);
}

// Copy the uint drawCount from counterBuffer into the first 4 bytes of commandBuffer
// so that glMultiDrawElementsIndirect can use the buffer with an initial counter slot.
void Renderer::copyCounterToIndirectBuffer()
{
    glBindBuffer(GL_COPY_READ_BUFFER, counterBuffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, commandBuffer);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(GLuint));
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

// Validate capacity of commandBuffer (debug helper)
bool Renderer::validateCommandBufferCapacity(size_t maxCommandsExpected)
{
    GLint bufferSize = 0;
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
    glGetBufferParameteriv(GL_DRAW_INDIRECT_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

    size_t capacity = (size_t)bufferSize / sizeof(DrawElementsIndirectCommand);
    // Reserve one slot if using the first uint as counter header
    return (capacity >= maxCommandsExpected + 1);
}

// High-level dispatch wrapper (commented template) that shows a safer compute->indirect flow.
void Renderer::dispatchCullingComputeAndDraw(Shader& computeShader,
                                            Shader& shader,
                                            Camera& camera,
                                            size_t numItems,
                                            GLuint currentVAO)
{
    // 1) Bind expected buffers
    bindGpuDrivenBuffersForCompute();

    // 2) Reset counter
    GLuint zero = 0;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // 3) Set compute uniforms
    computeShader.use();
    glm::mat4 viewProj = camera.GetProjectionMatrix(1.0f) * camera.GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(computeShader.ID, "uViewProj"), 1, GL_FALSE, glm::value_ptr(viewProj));
    glUniform1ui(glGetUniformLocation(computeShader.ID, "numItems"), (GLuint)numItems);
    // glUniform1ui(glGetUniformLocation(computeShader.ID, "maxCommands"),MAX_COMMANDS);

    // 4) Dispatch
    GLuint groups = (GLuint)((numItems + 63) / 64);
    glDispatchCompute(groups, 1, 1);

    // 5) Memory barrier to ensure SSBOs and indirect buffer are visible to draw
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);


    // 7) Read back drawCount for CPU-side decision
    GLuint drawCount = 0;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &drawCount);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    if (drawCount == 0) return;

    // 8) Execute indirect draw.
    glBindVertexArray(currentVAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);
    void* offset = (void*)0;
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, offset, drawCount, 0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::uploadRenderQueue(const std::vector<Renderer::GpuRenderItem>& items)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboRenderQueue);

    // allocate and upload full render queue
    glBufferData(GL_SHADER_STORAGE_BUFFER, items.size() * sizeof(Renderer::GpuRenderItem), items.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

std::vector<Renderer::GpuRenderItem>
Renderer::buildRenderQueue(Scene& scene)
{
    std::vector<Renderer::GpuRenderItem> queue;

    size_t totalInstances = 0;
    for (auto& g : scene.renderGroups)
        totalInstances += g.models.size();

    queue.reserve(std::min<size_t>(totalInstances, MAX_RENDER_ITEMS));

    for (auto& group : scene.renderGroups)
    {
        if (group.models.empty() || group.mesh == nullptr)
            continue;

        auto it = meshIndexMap.find(group.mesh);
        if (it == meshIndexMap.end())
        {
            std::cerr << "[WARN] mesh not found in meshIndexMap\n";
            continue;
        }

        uint32_t meshID = it->second;

        for (const auto& m : group.models)
        {
            Renderer::GpuRenderItem item{};

            item.meshID = meshID;
            item.materialID = 0;
            item.model = m;

            item.emissive = group.isEmissive ? 1u : 0u;
            item.emissiveColor = glm::vec4(group.emissiveColor, 0.0f);

            item.bounding = glm::vec4(
                group.mesh->boundingCenter,
                std::max(0.001f, group.mesh->boundingRadius)
            );

            queue.push_back(item);

            if (queue.size() >= MAX_COMMANDS)
                return queue;
        }
    }

    return queue;
}
    
// Build a global vertex/index buffer bundling meshes used by the scene.
// This populates `globalVBO`, `globalEBO`, configures `globalVAO`, and uploads
// a GPU-friendly `ssboMeshData` containing per-mesh (indexCount, firstIndex, baseVertex).
// It reads individual mesh VBOs (via Mesh::getVBO()) and concatenates their vertex data.
// For meshes without an element buffer, sequential indices are generated.
#if 1
void Renderer::buildGlobalMeshBuffer(Scene& scene)
{
    if (globalVAO == 0) initGlobalVAO();

    meshIndexMap.clear();
    gpuMeshes.clear();

    std::vector<uint8_t> vertexData;
    std::vector<uint32_t> indexData;

    uint32_t vertexBase = 0;
    uint32_t indexBase  = 0;

    const int stride = sizeof(float) * 8;

    for (auto& g : scene.renderGroups)
    {
        Mesh* mesh = g.mesh;
        if (!mesh) continue;

        if (meshIndexMap.count(mesh)) continue;

        uint32_t id = (uint32_t)gpuMeshes.size();
        meshIndexMap[mesh] = id;

        GLuint vbo = mesh->getVBO();
        GLuint ebo = mesh->getEBO();

        if (vbo == 0 || ebo == 0)
        {
            gpuMeshes.push_back({mesh->indexCount, mesh->firstIndex, mesh->baseVertex, 0});
            continue;
        }

        // vertex
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        GLint vsize = 0;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vsize);

        size_t old = vertexData.size();
        vertexData.resize(old + vsize);

        glGetBufferSubData(GL_ARRAY_BUFFER, 0, vsize, vertexData.data() + old);

        uint32_t vcount = vsize / stride;
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // index
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        GLint isize = 0;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &isize);

        std::vector<uint32_t> indices(isize / sizeof(uint32_t));
        glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, isize, indices.data());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        for (auto i : indices)
            indexData.push_back(i + vertexBase);

        gpuMeshes.push_back({
            (uint32_t)indices.size(),
            indexBase,
            vertexBase,
            0
        });

        vertexBase += vcount;
        indexBase  += indices.size();
    }

    // upload VAO buffers
    glBindVertexArray(globalVAO);

    glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size(), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * 4, indexData.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    // mesh SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMeshData);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        gpuMeshes.size() * sizeof(GpuMesh),
        gpuMeshes.data(),
        GL_STATIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboMeshData);

    globalMeshBuilt = true;
}
#endif
    
void Renderer::init_GPU()
{
    // =========================
    // UBO setup
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

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // =========================
    // 1. Render Queue SSBO (binding = 0)    
    glGenBuffers(1, &ssboRenderQueue);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboRenderQueue);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW); // dummy alloc
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboRenderQueue);

    // =========================
    // 2. Mesh SSBO (binding = 1)
    glGenBuffers(1, &ssboMeshData);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMeshData);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboMeshData);

    // =========================
    // 3. Counter SSBO (binding = 2)
    glGenBuffers(1, &counterBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);

    uint32_t zero = 0;
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t), &zero, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, counterBuffer);

    // =========================
    // 4. Indirect buffer
    glGenBuffers(1, &commandBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);

    glBufferData(GL_DRAW_INDIRECT_BUFFER,
        sizeof(GLuint) + MAX_COMMANDS * sizeof(DrawCommand),
        nullptr,
        GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

    // prepare global VAO/VBO/EBO for later consolidation
    // initGlobalVAO();

    // =========================
    // Shadow map
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, depthMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::init()
{
    // =========================
    // UBO setup
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
    glGenBuffers(1, &ssboRenderQueue);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboRenderQueue);

    // allocate small first（避免 size=0 undefined behavior）
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, nullptr, GL_DYNAMIC_DRAW);

    // binding = 8（要跟 compute shader 一致）
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, ssboRenderQueue);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // prepare global VAO/VBO/EBO
    // initGlobalVAO();

    // =========================
    // Shadow map
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
                      Shader& computeShader, Camera& camera,
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
    {
        // if (!globalMeshBuilt)
        // {
        //     buildGlobalMeshBuffer(scene);
        //     globalMeshBuilt = true;
        // }

        // auto queue = buildRenderQueue(scene);
        // uploadRenderQueue(queue);

        // dispatchCullingComputeAndDraw(computeShader, shader, camera, queue.size(), globalVAO);
        drawObjectsGPU(scene, shader);
        // drawObjectsGPU(scene, shader, computeShader, camera, width_, height_);        
    }
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

void Renderer::drawObjectsGPU(Scene& scene,
                              Shader& shader,
                              Shader& computeShader,
                              Camera& camera,
                              float width_,
                              float height_)
{
    shader.use();

    // =========================
    // 1. CPU build queue
    std::vector<RenderItem> queue;

    for (auto& group : scene.renderGroups)
    {
        for (auto& m : group.models)
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

    // =========================
    // 2. GPU buffers
    std::vector<GpuRenderItem> gpuItems;
    std::unordered_map<Mesh*, uint32_t> meshMap;
    std::vector<GpuMesh> gpuMeshes;

    for (auto& item : queue)
    {
        if (!meshMap.count(item.mesh))
        {
            meshMap[item.mesh] = gpuMeshes.size();

            gpuMeshes.push_back({
                item.mesh->indexCount,
                item.mesh->firstIndex,
                item.mesh->baseVertex,
                0
            });
        }

        gpuItems.push_back({
            item.model,
            glm::vec4(item.mesh->boundingCenter, item.mesh->boundingRadius),
            glm::vec4(item.emissiveColor, 0.0f),
            meshMap[item.mesh],
            0,
            item.isEmissive ? 1u : 0u,
            0
        });
    }

    // =========================
    // 3. Upload SSBOs
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboRenderQueue);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 gpuItems.size() * sizeof(GpuRenderItem),
                 gpuItems.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMeshData);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 gpuMeshes.size() * sizeof(GpuMesh),
                 gpuMeshes.data(),
                 GL_STATIC_DRAW);

    // =========================
    // 4. RESET drawCount
    uint32_t zero = 0;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t), &zero);

    // =========================
    // 5. COMPUTE
    computeShader.use();

    glm::mat4 viewProj =
        camera.GetProjectionMatrix(width_ / height_) *
        camera.GetViewMatrix();

    glUniformMatrix4fv(glGetUniformLocation(computeShader.ID, "uViewProj"),
                       1, GL_FALSE, glm::value_ptr(viewProj));

    glUniform1ui(glGetUniformLocation(computeShader.ID, "numItems"),
                 gpuItems.size());

    glUniform1f(glGetUniformLocation(computeShader.ID, "errorThreshold"),
                0.5f);

    glDispatchCompute((gpuItems.size() + 63) / 64, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                    GL_COMMAND_BARRIER_BIT);

    // =========================
    // 6. READ drawCount
    uint32_t drawCount = 0;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                       sizeof(uint32_t), &drawCount);

    if (drawCount == 0)
        return;

    // =========================
    // 7. INDIRECT DRAW
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer);

    // use consolidated global VAO (contains merged vertex/element data)
    glBindVertexArray(globalVAO);

    glMultiDrawElementsIndirect(
        GL_TRIANGLES,
        GL_UNSIGNED_INT,
        (void*)0,
        drawCount,
        0
    );
}


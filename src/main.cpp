#define GL_STATIC
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "core/Logger.h"
#include "core/ShaderLoader.h"
#include "graphics/Camera.h"
#include "graphics/Shader.h"
#include "scene/MeshFactory.h"

#include "scene/Scene.h"
#include "gpu/upload/SceneUploader.h"
#include "gpu/pipeline/VisibilityPipeline.h"
#include "gpu/backend/RendererGPU.h"
//"stb_image.h" first define in texture.h with  #define STB_IMAGE_IMPLEMENTATION

// =======================
// window
#define WIDTH 1280
#define HEIGHT 720

Camera camera(glm::vec3(0.0f, 0.0f, 6.0f),
              glm::vec3(0.0f, 1.0f, 0.0f),
              0.0f, -90.0f);

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
// =======================
// Camera
// =======================
unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{  glViewport(0, 0, width, height);  }

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // y reversed
    lastX = xpos; lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{  camera.ProcessMouseScroll(yoffset); }

void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(1, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(2, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(3, deltaTime);
}


#if 0
int main()
{
// 0  Meshes
// 1  Materials
// 2  Transforms
// 3  Instances (render)
// 4  DirLight
// 5  IndirectCommands
// 6  Counter
// 7  Debug
// 8  VisibleInstances (compute only)

    Logger::init();
    Logger::info("App start");

    // =======================
    // GLFW init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GPU Driven", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed window\n";
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // =======================
    // GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD failed\n";
        return -1;
    }

    std::cout << "GL: " << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_DEPTH_TEST);

    // =======================
    // Scene (CPU side empty for now)
    Scene scene;

    // =======================
    // GPU systems
    GPUScene gpuScene;
    VisibilitySystem visibility;
    RendererGPU renderer;

    // =======================
    // Mesh
    Mesh cube = MeshFactory::createCube();

    // ⚠️ 重要：避免 crash
    // 不在 constructor 做 shader 初始化
    renderer.init();

    // =======================
    // Shader (standalone test only)
    std::cout << "A: before shader\n";
    Shader basicShader = ShaderLoader::load("basic.vert", "basic.frag");
    std::cout << "B: after shader\n";
    basicShader.use();

    // =======================
    // MAIN LOOP
    while (!glfwWindowShouldClose(window))
    {
        float dt = 0.016f;
        processInput(window, dt);

        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = (float)WIDTH / (float)HEIGHT;

        // =========================
        // CAMERA → GPU UBO
        gpuScene.updateCamera(camera, aspect);

        // =========================
        // VISIBILITY (IMPORTANT)
        visibility.dispatchCulling(
            gpuScene.getInstanceBuffer().getID(),
            gpuScene.getTransformBuffer().getID(),
            gpuScene.getMeshBuffer().getID(),
            gpuScene.getCameraUBO().getID(), // ❗ we fix this below
            1 // temporary instanceCount
        );

        // =========================
        // RENDER PASS
        renderer.render(gpuScene, visibility);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    Logger::info("shutdown");
    Logger::shutdown();
    return 0;
}
#endif

#if 1
int main()
{
    Logger::init();
    Logger::info("App start");

    // =======================
    // GLFW init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GPU Driven", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD failed\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // =======================
    // GPU SYSTEMS
    SceneUploader scene;
    VisibilityPipeline visibility;
    RendererGPU renderer;

    scene.init();
    visibility.init();
    renderer.init();

    // =======================
    // MESH
    Mesh cube = MeshFactory::createCube();

    // =======================
    // VAO setup (IMPORTANT)
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 cube.vertices.size() * sizeof(Vertex),
                 cube.vertices.data(),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 cube.indices.size() * sizeof(uint32_t),
                 cube.indices.data(),
                 GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));

    // uv
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);

    // =======================
    // GPU MESH UPLOAD
    MeshDrawData gpuMesh{};
    gpuMesh.vertexOffset = 0;
    gpuMesh.indexOffset = 0;
    gpuMesh.vertexCount = cube.vertices.size();
    gpuMesh.indexCount = cube.indices.size();

    scene.uploadMeshes({ gpuMesh });

    // =======================
    // INSTANCE
    GPUObjectData inst{};
    inst.transformID = 0;
    inst.meshID = 0;
    inst.materialID = 0;
    inst.visibilityID = 0;

    scene.uploadInstances({ inst });

    // =======================
    // TRANSFORM
    GPUTransformData t{};
    t.model = glm::mat4(1.0f);

    scene.uploadTransforms({ t });

    // =======================
    // CAMERA SHADER TEST
    Shader basicShader = ShaderLoader::loadPass("basic.vert", "basic.frag");
    // =======================
    // MAIN LOOP
    while (!glfwWindowShouldClose(window))
    {
        processInput(window, 0.016f);

        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = (float)WIDTH / (float)HEIGHT;

        // camera → GPU
        scene.updateCamera(camera, aspect);

        // =======================
        // GPU PIPELINE
        visibility.dispatchCulling(scene, 1);

        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glFinish();

        GLuint v = 0;

        glBindBuffer(GL_SHADER_STORAGE_BUFFER,
                    visibility.getCounterBuffer());

        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER,
                            0,
                            sizeof(GLuint),
                            &v);

        std::cout << "CPU read = " << v << std::endl;

        renderer.render(scene, visibility, vao);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
#endif


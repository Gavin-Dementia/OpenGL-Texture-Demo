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
#include "gpu/backend/RendererGPU.h"
#include "gpu/layout/GPUObjectData.h"
#include "gpu/layout/GPUTransformData.h"
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

#if 1
int main()
{
    Logger::init();
    Logger::info("App start");

    // =========================================================
    // 1. INIT CONTEXT (GLFW + GLAD)
    GLFWwindow* window = nullptr;

    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "GPU Driven", nullptr, nullptr);
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
    }

    // =========================================================
    // 2. ENGINE SYSTEMS INIT
    BindingManager binding;
    SceneUploader uploader;

    RendererGPU renderer;
    VisibilityPass visibility;
    DrawPass draw;

    Scene scene;

    std::cout << "===== init uploader =====\n";
    uploader.init();

    std::cout << "===== init visibility =====\n";
    visibility.init();

    std::cout << "===== init draw =====\n";
    draw.init();

    std::cout << "===== init renderer =====\n";
    renderer.init();

    std::cout << "===== init done =====\n";

    // =========================================================
    // 3. SCENE BUILD (CPU SIDE ONLY)
    Mesh cube = MeshFactory::createCube();

    SceneInstance inst;
    inst.mesh = &cube;
    inst.transform = glm::mat4(1.0f);
    inst.material = nullptr;

    scene.instances.push_back(inst);

    // =========================================================
    // 4. GPU UPLOAD PHASE
    MeshDrawData gpuDraw = uploader.uploadMesh(cube);
    uploader.uploadDrawData(gpuDraw);

    std::vector<GPUObjectData> gpuObjects;
    gpuObjects.reserve(scene.instances.size());

    for (auto& s : scene.instances)
    {
        GPUObjectData obj{};
        obj.transformID = 0;
        obj.meshID = 0;
        obj.materialID = 0;
        obj.visibilityID = 0;

        gpuObjects.push_back(obj);
    }

    uploader.uploadObjects(gpuObjects);
    uploader.uploadTransforms({ glm::mat4(1.0f) });

    // =========================================================
    // 5. VAO SETUP (STATIC)
    GLuint vao = 0, vbo = 0, ebo = 0;

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

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);

    // bind once
    draw.bind(vao,
              uploader.getDrawBuffer(),
              uploader.getCounterBuffer());

    // =========================================================
    // 6. MAIN LOOP
    do
    {
        processInput(window, 0.016f);

        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = (float)WIDTH / (float)HEIGHT;
        uploader.updateCamera(camera, aspect);

        std::cout << "===== frame begin =====\n";

        renderer.render(
            scene,
            uploader,
            binding,
            draw,
            vao
        );

        std::cout << "===== frame end =====\n";

        glfwSwapBuffers(window);
        glfwPollEvents();
    }while (!glfwWindowShouldClose(window));

//     while (!glfwWindowShouldClose(window))
// {
//     glfwPollEvents();

//     int w,h;
//     glfwGetFramebufferSize(window, &w, &h);

//     std::cout << "FB size: " << w << "," << h << std::endl;

//     glViewport(0,0,w,h);

//     glClearColor(1,0,0,1);
//     glClear(GL_COLOR_BUFFER_BIT);

//     glfwSwapBuffers(window);
// }

    // =========================================================
    // 7. SHUTDOWN
    glfwTerminate();
    return 0;
}
#endif


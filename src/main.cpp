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

#include "scene/Scene.h"
#include "gpu/scene/GPUScene.h"
#include "gpu/visibility/VisibilitySystem.h"
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


float cubeVertices[] =
{
    // positions
    -0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,

    -0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f
};

unsigned int cubeIndices[] =
{
    0,1,2, 2,3,0,
    4,5,6, 6,7,4,
    0,4,7, 7,3,0,
    1,5,6, 6,2,1,
    3,2,6, 6,7,3,
    0,1,5, 5,4,0
};

// int main()
// {
//     glfwInit();

//     GLFWwindow* window =
//         glfwCreateWindow(800, 600, "test", nullptr, nullptr);

//     glfwMakeContextCurrent(window);
//     gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

//     std::cout << "GL OK\n";

//     while (!glfwWindowShouldClose(window))
//     {
//         glClear(GL_COLOR_BUFFER_BIT);
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }
// }

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

        // gpuScene.upload(scene);
        // gpuScene.updateCamera(camera, aspect);
        // visibility.runCullCompute(...);

        // =====================================================
        // SAFE RENDER PATH
        basicShader.use();

        // dummy draw
        glBegin(GL_TRIANGLES);
        glVertex3f(-0.5f, -0.5f, 0.0f);
        glVertex3f( 0.5f, -0.5f, 0.0f);
        glVertex3f( 0.0f,  0.5f, 0.0f);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    Logger::info("shutdown");
    Logger::shutdown();
    return 0;
}
#endif


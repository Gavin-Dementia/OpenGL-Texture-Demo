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
    MeshDrawData gpuDraw{};
    gpuDraw.vertexOffset = 0;
    gpuDraw.indexOffset = 0;
    gpuDraw.vertexCount = cube.vertices.size();
    gpuDraw.indexCount = cube.indices.size();

    scene.uploadDrawData(gpuDraw);

//     std::vector<glm::vec4> bounds;
// bounds.push_back(glm::vec4(
//     mesh.boundingCenter,
//     mesh.boundingRadius
// ));

// scene.uploadMeshBounds(bounds);
    
    // =======================
    // INSTANCE
    GPUObjectData object{};
    object.transformID = 0;
    object.meshID = 0;
    object.materialID = 0;
    object.visibilityID = 0;

    scene.uploadObjects(object);

    // =======================
    // TRANSFORM
    GPUTransformData t{};
    t.model = glm::mat4(1.0f);

    scene.uploadTransforms(t);

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


#include <iostream>
#include <cmath> 

#define GL_STATIC
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Texture.h"
#include "Cube.h"
#include "Scene.h"
#include "Renderer.h"
//"stb_image.h" first define in texture.h with  #define STB_IMAGE_IMPLEMENTATION

#define width_ 1260
#define height_ 1080

unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{  glViewport(0, 0, width, height);  }

float lastX = 400, lastY = 300;
bool firstMouse = true;

// Camera camera(glm::vec3(0.0f, 3.0f, 3.0f), 
//               glm::vec3(0.0f, 0.0f, 0.0f), 
//               glm::vec3(0.0f, 1.0f, 0.0f));
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f),
              glm::vec3(0.0f, 1.0f, 0.0f), 
              0.0f, -90.0f);

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
{// Initialize GLFW
    // ---------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
    // Create GLFW window
    // ---------------------------
    GLFWwindow* window = glfwCreateWindow(width_, height_, "Learn OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Load OpenGL function pointers with GLAD
    // ---------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    glViewport(0, 0, width_, height_);    
    glEnable(GL_DEPTH_TEST);// Enable depth testing
    // stbi_set_flip_vertically_on_load(true);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Input callbacks
    // ---------------------------
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window,  scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 隱藏滑鼠

    // =======================
    // Shader / Texture
    // =======================
    Shader shader("C:/3Dproject/shaders/basic.vert", "C:/3Dproject/shaders/basic.frag");
    Shader lightShader("C:/3Dproject/shaders/light.vert", "C:/3Dproject/shaders/light.frag");
    Texture diffuseTex("C:/3Dproject/box.png");
    Texture specularTex("C:/3Dproject/box_specular.png");

    // =======================
    // Scene / Renderer
    // =======================
    Scene scene;
    Renderer renderer;

    Cube* cube = new Cube();

    // cube positions
    glm::vec3 cubePositions[9];
    int idx = 0;
    float spacing = 2.5f;

    for (int r = 0; r < 3; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            cubePositions[idx++] = glm::vec3(
                (c - 1.5f) * spacing,
                0.0f,
                (1.5f - r) * spacing
            );
        }
    }

    // =======================
    // Add objects to Scene
    // =======================
    for (int i = 0; i < 9; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);

        scene.objects.push_back({cube, model});
    }

    // =======================
    // Lights
    // =======================
    scene.lights.dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    scene.lights.dirLight.ambient  = glm::vec3(0.05f);
    scene.lights.dirLight.diffuse  = glm::vec3(0.4f);
    scene.lights.dirLight.specular = glm::vec3(0.5f);

    for (int i = 0; i < 2; i++)
    {
        scene.lights.pointLights.push_back({
            cubePositions[i],
            1.0f, 0.09f, 0.032f,
            glm::vec3(0.05f),
            glm::vec3(0.8f),
            glm::vec3(1.0f)
        });
    }

    // =======================
    // Timing
    // =======================
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // =======================
    // Render Loop
    // =======================
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        scene.update(deltaTime);

        // bind textures（交給 renderer 前）
        shader.use();
        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", 128.0f);

        diffuseTex.bind(0);
        specularTex.bind(1);

        renderer.render(scene, shader, lightShader, camera, width_, height_);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete cube;
    glfwTerminate();
    return 0;
}

#endif


#include <iostream>
#include <cmath> 

#define GL_STATIC
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"
#include "Cube.h"
#include "Scene.h"
#include "Renderer.h"
//"stb_image.h" first define in texture.h with  #define STB_IMAGE_IMPLEMENTATION

#define width_ 1260
#define height_ 1080
// =======================
// Camera
// =======================
float lastX = 400, lastY = 300;
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f),
              glm::vec3(0.0f, 1.0f, 0.0f), 
              0.0f, -90.0f);
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

void buildScene(Scene& scene, Cube& cube)
{
    RenderGroup group;
    group.mesh = &cube;
    group.models.reserve(9);

    float spacing = 2.5f;

    for (int r = 0; r < 3; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(
                model,
                glm::vec3(
                    (c - 1.5f) * spacing,
                    0.0f,
                    (1.5f - r) * spacing
                )
            );

            group.models.push_back(model);
        }
    }

    scene.renderGroups.push_back(group);
    // =======================
    // Lights
    // =======================

    // Directional Light
    scene.lights.dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    scene.lights.dirLight.ambient   = glm::vec3(0.05f);
    scene.lights.dirLight.diffuse   = glm::vec3(0.4f);
    scene.lights.dirLight.specular  = glm::vec3(0.5f);
    // populate GPU cache (std140 vec4 fields)
    scene.lights.dirLightGPU.direction = glm::vec4(scene.lights.dirLight.direction, 0.0f);
    scene.lights.dirLightGPU.ambient   = glm::vec4(scene.lights.dirLight.ambient, 0.0f);
    scene.lights.dirLightGPU.diffuse   = glm::vec4(scene.lights.dirLight.diffuse, 0.0f);
    scene.lights.dirLightGPU.specular  = glm::vec4(scene.lights.dirLight.specular, 0.0f);
    std::cout << "[buildScene] dirLight set to: "
              << scene.lights.dirLight.direction.x << ", "
              << scene.lights.dirLight.direction.y << ", "
              << scene.lights.dirLight.direction.z << std::endl;
    // =======================
    // SpotLight (initial values)
    scene.lights.spotLight.position = camera.Position;
    scene.lights.spotLight.direction = glm::normalize(camera.Forward);
    scene.lights.spotLight.cutOff = glm::cos(glm::radians(12.5f));
    scene.lights.spotLight.outerCutOff = glm::cos(glm::radians(17.5f));
    scene.lights.spotLight.constant = 1.0f;
    scene.lights.spotLight.linear = 0.09f;
    scene.lights.spotLight.quadratic = 0.032f;
    scene.lights.spotLight.ambient = glm::vec3(0.05f);
    scene.lights.spotLight.diffuse = glm::vec3(1.0f);
    scene.lights.spotLight.specular = glm::vec3(1.0f);
    std::cout << "[buildScene] spotLight pos: "
              << scene.lights.spotLight.position.x << ", "
              << scene.lights.spotLight.position.y << ", "
              << scene.lights.spotLight.position.z << std::endl;

    // Point lights
    std::vector<glm::vec3> lightPositions =
    {
        { 0.7f,  0.2f,  2.0f },
        { 2.3f, -3.3f, -4.0f }
    };

    for (auto& pos : lightPositions)
    {
        scene.lights.pointLights.push_back({
            pos,
            1.0f, 0.09f, 0.032f,
            glm::vec3(0.05f),
            glm::vec3(0.8f),
            glm::vec3(1.0f)
        });

        // light debug visuals (still non-instanced)
        Object obj;
        obj.mesh = &cube;

        obj.position = pos; 
        obj.scale = glm::vec3(0.2f); 

        obj.isEmissive = true;
        obj.emissiveColor = glm::vec3(1.0f);

        scene.lightVisuals.push_back(obj);
    }

    // mark lights dirty so first upload initializes UBOs
    scene.lights.dirDirty = true;
    scene.lights.spotDirty = true;
    scene.lights.pointLightDirty.resize(scene.lights.pointLights.size(), 1);
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
    Shader shader("C:/3Dproject/shaders/basic.vert",
                  "C:/3Dproject/shaders/basic.frag");
    Shader lightShader("C:/3Dproject/shaders/light.vert",
                       "C:/3Dproject/shaders/light.frag");
    Shader depthShader("C:/3Dproject/shaders/depth.vert",
                       "C:/3Dproject/shaders/depth.frag");
    // Bind uniform blocks to binding points (std140 UBOs)
    // Camera -> binding 0, DirLightBlock -> binding 1
    {
        GLint idx = glGetUniformBlockIndex(shader.ID, "Camera");
        if (idx != GL_INVALID_INDEX) glUniformBlockBinding(shader.ID, idx, 0);
        idx = glGetUniformBlockIndex(shader.ID, "DirLightBlock");
        if (idx != GL_INVALID_INDEX) glUniformBlockBinding(shader.ID, idx, 1);
        idx = glGetUniformBlockIndex(shader.ID, "PointLightsBlock");
        if (idx != GL_INVALID_INDEX) glUniformBlockBinding(shader.ID, idx, 2);
        idx = glGetUniformBlockIndex(shader.ID, "SpotLightBlock");
        if (idx != GL_INVALID_INDEX) glUniformBlockBinding(shader.ID, idx, 3);

        idx = glGetUniformBlockIndex(lightShader.ID, "Camera");
        if (idx != GL_INVALID_INDEX) glUniformBlockBinding(lightShader.ID, idx, 0);
        idx = glGetUniformBlockIndex(lightShader.ID, "DirLightBlock");
        if (idx != GL_INVALID_INDEX) glUniformBlockBinding(lightShader.ID, idx, 1);
        idx = glGetUniformBlockIndex(lightShader.ID, "PointLightsBlock");
        if (idx != GL_INVALID_INDEX) glUniformBlockBinding(lightShader.ID, idx, 2);
        idx = glGetUniformBlockIndex(lightShader.ID, "SpotLightBlock");
        if (idx != GL_INVALID_INDEX) glUniformBlockBinding(lightShader.ID, idx, 3);
    }
    Texture diffuseTex("C:/3Dproject/box.png");
    Texture specularTex("C:/3Dproject/box_specular.png");

    // =======================
    // Scene / Renderer
    // =======================
    Scene scene;
    Renderer renderer;
    Cube cube;
    // =======================
    // Build Scene
    // =======================
    buildScene(scene, cube);
    // assign a default material to the first render group (example)
    Material* defaultMat = new Material(&diffuseTex, &specularTex, 128.0f);
    if (!scene.renderGroups.empty())
        scene.renderGroups[0].material = defaultMat;
    // Initialize renderer (create UBOs, bind ranges)
    renderer.init();
    // =======================
    // Timing
    // =======================
    float lastFrame = 0.0f;

    // =======================
    // Render Loop
    // =======================
    while (!glfwWindowShouldClose(window))
    {
        float current = glfwGetTime();
        float dt = current - lastFrame;
        lastFrame = current;

        processInput(window, dt);
        // scene.update(dt);//anime

        shader.use();
        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", 128.0f);

        diffuseTex.bind(0);
        specularTex.bind(1);

        renderer.render(scene, shader, lightShader, depthShader, camera, width_, height_);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif


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
{
    // Initialize GLFW
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

    // Load shaders and textures
    // ---------------------------
    Shader* testshader   = new Shader("C:/3Dproject/shaders/basic.vert", "C:/3Dproject/shaders/basic.frag");
    Shader* lightShader  = new Shader("C:/3Dproject/shaders/light.vert", "C:/3Dproject/shaders/light.frag");
    Texture* boxTexture  = new Texture("C:/3Dproject/box.png");
    Texture* box_specular  = new Texture("C:/3Dproject/box_specular.png");
    Cube cube;// Our cube mesh class
    
    // Cube positions in a 3x3 grid
    // ---------------------------
    glm::vec3 cubePositions[9];
    int index = 0;
    float spacing = 2.5f;
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            cubePositions[index++] = glm::vec3(
                (col - 1.5f) * spacing,  // center along x
                0.0f,                     // y
                (1.5f - row) * spacing    // z (forward)
            );
        }
    }

    // Light cube setup
    // ---------------------------
    float lightCubeVertices[] = {
        // back face
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // front face
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // left face
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        // right face
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

        // bottom face
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        // top face
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVertices), lightCubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);   // position attribute 
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)) );
    // glEnableVertexAttribArray(1);   // color attribute
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)) );
    // glEnableVertexAttribArray(2);   // texture coord attribute
    // glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)) );
    // glEnableVertexAttribArray(3);   // Normal coord attribute

    // Timing variables
    // ---------------------------    
    float deltaTime = 0.0f; // time between current frame and last frame
    float lastFrame = 0.0f;

    // Render loop
    // ---------------------------
    while(!glfwWindowShouldClose(window))
    {        
        // Frame timing
        // ---------------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, deltaTime);// input

        // Clear screen
        // ---------------------------
        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        float time = glfwGetTime();

        // Update light position (moving light)
        // ---------------------------
        glm::vec3 lightPos;
        // lightPos.x = sin(time) * 3.5f;
        // lightPos.y = sin(time) * 3.0f;
        // lightPos.z = cos(time) * 3.5f;
        lightPos.x = -sin(1.0f) * 1.5f;
        lightPos.y = sin(1.0f) * 1.5f;
        lightPos.z = cos(0.0f) * 1.5f;
        // Update light color (dynamic color)
        // ---------------------------
        glm::vec3 lightColor;
        // lightColor.x = sin(glfwGetTime() * 1.0f);
        // lightColor.y = sin(glfwGetTime() * 0.7f);
        // lightColor.z = sin(glfwGetTime() * 0.3f);
        lightColor.x = 1.0f;
        lightColor.y = 1.0f;
        lightColor.z = 1.0f;
        
        glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); 
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); 

        // Camera matrices
        // ---------------------------
        glm::mat4 projMat = glm::perspective(glm::radians((camera.Zoom)), (float)width_/(float)height_, 0.1f, 100.0f);
        glm::mat4 viewMat = camera.GetViewMatrix(); 

        // Draw cubes with lighting shader
        // ---------------------------
        testshader->use();
        testshader->setMat4("viewMat", viewMat);
        testshader->setMat4("projMat", projMat);
        testshader->setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        testshader->setVec3("cameraPos", camera.Position);
        // testshader->setVec3("material.ambient", 0.1f, 0.1f, 0.1f);
        testshader->setInt("material.diffuse", 0);//boxTexture
        // testshader->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        // testshader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        testshader->setInt("material.specular", 1);//box_specular
        testshader->setFloat("material.shininess", 128.0f);

        testshader->setFloat("light.constant",  1.0f);
        testshader->setFloat("light.linear",    0.1f);
        testshader->setFloat("light.quadratic", 0.0016f);

        testshader->setVec3("light.position", lightPos);
        testshader->setVec3("light.ambient", ambientColor);
        testshader->setVec3("light.diffuse", diffuseColor);
        testshader->setVec3("light.specular", glm::vec3(0.2f)); // 白光
        // testshader->setInt("ourTextureF", 3);//frogTexture
        boxTexture->bind(0);
        box_specular->bind(1);
       
        for (unsigned int i = 0; i < 9; ++i)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, cubePositions[i]);
            // modelMat = glm::rotate(modelMat, glm::radians(sin(time) *5 * i), glm::vec3(1.0f, 0.3f, 0.5f));

            testshader->setMat4("modelMat", modelMat);        
            testshader->setMat4("viewMat", viewMat);
            testshader->setMat4("projMat", projMat);

            cube.draw();
        }        
        // Draw light cube
        // ---------------------------
        lightShader->use();
        glm::mat4 modelLight = glm::mat4(1.0f);
        modelLight = glm::translate(modelLight, lightPos);
        modelLight = glm::scale(modelLight, glm::vec3(0.2f));
        lightShader->setMat4("modelLight", modelLight);
        lightShader->setMat4("viewMat", viewMat);
        lightShader->setMat4("projMat", projMat);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        // Swap buffers and poll events
        // ---------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    // ---------------------------
    delete boxTexture;
    delete box_specular;
    delete testshader;
    delete lightShader;

    glfwTerminate();
    return 0;
}
#endif
//Resource Manager
#if 0
int main()//test
{
   glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
   glm::mat4 trans;

   trans= glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
   vec= trans * vec;

   std::cout << vec.x << vec.y << vec.z << std::endl;

    return 0;
}
#endif


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

// Callback: 視窗大小改變
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
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
    // 建立窗口
    GLFWwindow* window = glfwCreateWindow(width_, height_, "Learn OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    glViewport(0, 0, width_, height_);    
    glEnable(GL_DEPTH_TEST);
    // stbi_set_flip_vertically_on_load(true);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window,  scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 隱藏滑鼠

    Shader* testshader   = new Shader("C:/3Dproject/shaders/basic.vert", "C:/3Dproject/shaders/basic.frag");
    Shader* lightShader  = new Shader("C:/3Dproject/shaders/light.vert", "C:/3Dproject/shaders/light.frag");
    Texture* wallTexture = new Texture("C:/3Dproject/wall.jpg");
    Texture* frogTexture = new Texture("C:/3Dproject/frog.jpg");
    Cube cube;

    glm::vec3 cubePositions[16];

    int index = 0;
    float spacing = 2.5f;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            cubePositions[index++] = glm::vec3(
                (col - 1.5f) * spacing,  // 置中
                0.0f,
                (1.5f - row) * spacing   // 往前排
            );
        }
    }

    float lightCubeVertices[] = {
        // positions          
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        // ... repeat for 6 faces
        };

    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVertices), lightCubeVertices, GL_STATIC_DRAW);

    // 只綁定 position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);   // position attribute 
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)) );
    // glEnableVertexAttribArray(1);   // color attribute
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)) );
    // glEnableVertexAttribArray(2);   // texture coord attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)) );
    glEnableVertexAttribArray(3);   // Normal coord attribute
    
    float deltaTime = 0.0f; // time between current frame and last frame
    float lastFrame = 0.0f;

     // 渲染循環
    while(!glfwWindowShouldClose(window))
    {        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, deltaTime);// input

        // render 
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        float time = glfwGetTime();

        glm::vec3 lightPos;
        lightPos.x = sin(time) * 5.0f;
        lightPos.y = 2.0f;
        lightPos.z = cos(time) * 5.0f;

        glm::mat4 projMat = glm::perspective(glm::radians((camera.Zoom)), (float)width_/(float)height_, 0.1f, 100.0f);
        glm::mat4 viewMat = camera.GetViewMatrix(); 

        testshader->use();
        // testshader->setInt("ourTextureW", 0);//wallTexture
        // testshader->setInt("ourTextureF", 3);//frogTexture
        // wallTexture->bind(0);
        // frogTexture->bind(3);
        glUniform3f(glGetUniformLocation(testshader->ID, "objectColor"), 1.0f, 0.5f, 0.3f);
        glUniform3f(glGetUniformLocation(testshader->ID, "ambientColor"), 0.1f, 0.0f, 0.2f);
        // glUniform3f(glGetUniformLocation(testshader->ID, "lightPos"), 10.f, 10.f, -5.0f);
        glUniform3f(glGetUniformLocation(testshader->ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(testshader->ID, "lightColor"), 0.9f, 0.9f, 0.9f);
        glUniform3f(glGetUniformLocation(testshader->ID, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);

        // glBindVertexArray(VAO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
       
        for (unsigned int i = 0; i < 16; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, cubePositions[i]);
            modelMat = glm::rotate(modelMat, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));

            testshader->setMat4("modelMat", modelMat);        
            testshader->setMat4("viewMat", viewMat);
            testshader->setMat4("projMat", projMat);

            cube.draw();
        }        

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

        // 事件與緩衝
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cin.get();
    delete wallTexture;
    delete frogTexture;
    delete testshader;
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


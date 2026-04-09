#ifndef CUBE_H
#define CUBE_H

#include <glad/glad.h>

class Cube
{
public:
    Cube()
    {
        float vertices[] = {
            // position          // normal
            -0.5f,-0.5f,-0.5f,  0.0f,0.0f,-1.0f,
             0.5f,-0.5f,-0.5f,  0.0f,0.0f,-1.0f,
             0.5f, 0.5f,-0.5f,  0.0f,0.0f,-1.0f,
             0.5f, 0.5f,-0.5f,  0.0f,0.0f,-1.0f,
            -0.5f, 0.5f,-0.5f,  0.0f,0.0f,-1.0f,
            -0.5f,-0.5f,-0.5f,  0.0f,0.0f,-1.0f,

            -0.5f,-0.5f, 0.5f,  0.0f,0.0f,1.0f,
             0.5f,-0.5f, 0.5f,  0.0f,0.0f,1.0f,
             0.5f, 0.5f, 0.5f,  0.0f,0.0f,1.0f,
             0.5f, 0.5f, 0.5f,  0.0f,0.0f,1.0f,
            -0.5f, 0.5f, 0.5f,  0.0f,0.0f,1.0f,
            -0.5f,-0.5f, 0.5f,  0.0f,0.0f,1.0f,

            -0.5f, 0.5f, 0.5f, -1.0f,0.0f,0.0f,
            -0.5f, 0.5f,-0.5f, -1.0f,0.0f,0.0f,
            -0.5f,-0.5f,-0.5f, -1.0f,0.0f,0.0f,
            -0.5f,-0.5f,-0.5f, -1.0f,0.0f,0.0f,
            -0.5f,-0.5f, 0.5f, -1.0f,0.0f,0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f,0.0f,0.0f,

             0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,
             0.5f, 0.5f,-0.5f,  1.0f,0.0f,0.0f,
             0.5f,-0.5f,-0.5f,  1.0f,0.0f,0.0f,
             0.5f,-0.5f,-0.5f,  1.0f,0.0f,0.0f,
             0.5f,-0.5f, 0.5f,  1.0f,0.0f,0.0f,
             0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,

            -0.5f,-0.5f,-0.5f,  0.0f,-1.0f,0.0f,
             0.5f,-0.5f,-0.5f,  0.0f,-1.0f,0.0f,
             0.5f,-0.5f, 0.5f,  0.0f,-1.0f,0.0f,
             0.5f,-0.5f, 0.5f,  0.0f,-1.0f,0.0f,
            -0.5f,-0.5f, 0.5f,  0.0f,-1.0f,0.0f,
            -0.5f,-0.5f,-0.5f,  0.0f,-1.0f,0.0f,

            -0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,
             0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f,
             0.5f, 0.5f, 0.5f,  0.0f,1.0f,0.0f,
             0.5f, 0.5f, 0.5f,  0.0f,1.0f,0.0f,
            -0.5f, 0.5f, 0.5f,  0.0f,1.0f,0.0f,
            -0.5f, 0.5f,-0.5f,  0.0f,1.0f,0.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    
    ~Cube()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    
    void draw()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    unsigned int VAO, VBO;
};

#endif


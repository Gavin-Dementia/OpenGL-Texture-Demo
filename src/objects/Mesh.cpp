#include "Mesh.h"

void Mesh::setInstances(const std::vector<glm::mat4>& models)
{
    glBindVertexArray(VAO); // ⭐ critical

    if (instanceVBO == 0)
        setupInstanceBuffer();

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    glBufferData(GL_ARRAY_BUFFER,
                 models.size() * sizeof(glm::mat4),
                 models.data(),
                 GL_STATIC_DRAW);

    instanceCount = models.size();

    glBindVertexArray(0);
}

void Mesh::setupInstanceBuffer()
{
    glBindVertexArray(VAO);

    glGenBuffers(1, &instanceVBO);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // ✔ required

    std::size_t vec4Size = sizeof(glm::vec4);

    for (int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(
            3 + i,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::mat4),
            (void*)(i * vec4Size)
        );

        glVertexAttribDivisor(3 + i, 1);
    }

    glBindVertexArray(0);
}


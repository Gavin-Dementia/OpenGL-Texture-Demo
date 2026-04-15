#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

class Mesh
{
public:
    virtual ~Mesh() = default;

    virtual void draw() = 0;
    virtual void drawInstanced(int count) = 0;

    void setupInstanceBuffer();
    void setInstances(const std::vector<glm::mat4>& models);

protected:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int instanceVBO = 0;

    int instanceCount = 0;
};

#endif


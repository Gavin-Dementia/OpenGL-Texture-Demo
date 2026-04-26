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

    uint32_t indexCount = 0;
    uint32_t firstIndex = 0;
    uint32_t baseVertex = 0;
    
    glm::vec3 boundingCenter = glm::vec3(0.0f);
    float boundingRadius = 1.0f;

protected:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int instanceVBO = 0;

    int instanceCount = 0;
public:
    unsigned int getVAO() const { return VAO; }
    unsigned int getVBO() const { return VBO; }
    unsigned int getInstanceVBO() const { return instanceVBO; }
};

#endif


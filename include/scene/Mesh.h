#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include "gpu/layout/Vertex.h"

class Mesh
{
public:

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // GPU global buffer offsets
    uint32_t firstIndex = 0;
    uint32_t baseVertex = 0;
    uint32_t indexCount = 0;

    // Bounding sphere
    glm::vec3 boundingCenter = glm::vec3(0.0f);
    float boundingRadius = 1.0f;

public:

    Mesh() = default;

    virtual ~Mesh() = default;

    void computeBounds();

};

#endif
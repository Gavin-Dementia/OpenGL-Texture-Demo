#include "scene/Mesh.h"

#include <glm/gtx/norm.hpp>
#include <algorithm>

// ============================================
// Compute bounding sphere
void Mesh::computeBounds()
{
    if (vertices.empty())
        return;

    // ========================================
    // Compute AABB

    glm::vec3 minP = vertices[0].position;
    glm::vec3 maxP = vertices[0].position;

    for (const auto& v : vertices)
    {
        minP.x = std::min(minP.x, v.position.x);
        minP.y = std::min(minP.y, v.position.y);
        minP.z = std::min(minP.z, v.position.z);

        maxP.x = std::max(maxP.x, v.position.x);
        maxP.y = std::max(maxP.y, v.position.y);
        maxP.z = std::max(maxP.z, v.position.z);
    }

    // ========================================
    // Sphere center

    boundingCenter = (minP + maxP) * 0.5f;

    // ========================================
    // Radius
    float maxDistSq = 0.0f;

    for (const auto& v : vertices)
    {
        float distSq =
            glm::length2(v.position - boundingCenter);

        if (distSq > maxDistSq)
            maxDistSq = distSq;
    }

    boundingRadius = std::sqrt(maxDistSq);
}
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

    glm::vec3 minP = vertices[0].pos;
    glm::vec3 maxP = vertices[0].pos;

    for (const auto& v : vertices)
    {
        minP.x = std::min(minP.x, v.pos.x);
        minP.y = std::min(minP.y, v.pos.y);
        minP.z = std::min(minP.z, v.pos.z);

        maxP.x = std::max(maxP.x, v.pos.x);
        maxP.y = std::max(maxP.y, v.pos.y);
        maxP.z = std::max(maxP.z, v.pos.z);
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
            glm::length2(v.pos - boundingCenter);

        if (distSq > maxDistSq)
            maxDistSq = distSq;
    }

    boundingRadius = std::sqrt(maxDistSq);
}
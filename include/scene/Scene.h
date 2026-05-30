#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include <glm/glm.hpp>

#include "scene/Mesh.h"
#include "scene/Material.h"

// ============================================
// Scene Instance
struct SceneInstance
{
    Mesh* mesh;
    Material* material;
    glm::mat4 transform;
};

// ============================================
// Scene
class Scene
{
public:

    std::vector<SceneInstance> instances;

    size_t getInstanceCount() const
    {
        return instances.size();
    }
};

#endif // SCENE_H


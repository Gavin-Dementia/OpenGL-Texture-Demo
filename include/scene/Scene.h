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
    Mesh* mesh = nullptr;

    Material* material = nullptr;

    glm::mat4 transform = glm::mat4(1.0f);

    bool visible = true;
};

// ============================================
// Scene
class Scene
{
public:

    std::vector<SceneInstance> instances;

};

#endif // SCENE_H


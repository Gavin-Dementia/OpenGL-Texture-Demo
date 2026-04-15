#include "Scene.h"
#include <cmath>

void Scene::update(float deltaTime)
{
    for (auto& l : lights.pointLights)
    {
        l.position.x = std::sin(deltaTime) * 3.5f;
        l.position.y = std::sin(deltaTime) * 3.0f;
        l.position.z = std::cos(deltaTime) * 3.5f;
    }
}


#include "Scene.h"
#include <cmath>

void Scene::update(float deltaTime)
{
    static float acc = 0.0f;
    acc += deltaTime;

    for (auto& l : lights.pointLights)
    {
        l.position.x = std::sin(acc) * 3.5f;
        l.position.y = std::sin(acc) * 3.0f;
        l.position.z = std::cos(acc) * 3.5f;
    }

    // Sync debug visuals with point light positions
    size_t n = std::min(lights.pointLights.size(), lightVisuals.size());
    for (size_t i = 0; i < n; ++i)
    {
        lightVisuals[i].position = lights.pointLights[i].position;
    }
}


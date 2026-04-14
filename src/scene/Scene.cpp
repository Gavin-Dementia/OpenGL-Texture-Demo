#include "Scene.h"

void Scene::update(float deltaTime)
{
    // animation / light movement / physics
    for (auto& l : pointLights)
        {
            // example animation
            // Update light position (moving light)
            // ---------------------------
            glm::vec3 lightPos;
            lightPos.x = sin(deltaTime) * 3.5f;
            lightPos.y = sin(deltaTime) * 3.0f;
            lightPos.z = cos(deltaTime) * 3.5f;
            // lightPos.x = -sin(1.0f) * 1.5f;
            // lightPos.y = sin(1.0f) * 1.5f;
            // lightPos.z = cos(0.0f) * 1.5f;
        }
}
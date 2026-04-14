#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"

// =======================
// Light structs
// =======================
struct DirLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

// =======================
// Light Manager
// =======================
struct LightManager
{
    DirLight dirLight;
    std::vector<PointLight> pointLights;
    SpotLight spotLight;
};

// =======================
// Object struct
// =======================
struct Object
{
    Mesh* mesh;
    glm::mat4 model;
};

// =======================
// Scene
// =======================
class Scene
{
public:
    std::vector<Object> objects;
    LightManager lights;

    Scene() = default;

    DirLight dirLight;
    std::vector<PointLight> pointLights;
    SpotLight spotLight;

    // debug lights (visualization)
    std::vector<Object> lightObjects;

    void update(float deltaTime);
};

#endif
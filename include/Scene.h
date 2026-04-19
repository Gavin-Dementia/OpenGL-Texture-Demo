#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Mesh.h"

// =======================
// Light structs
// =======================
struct DirLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float intensity = 1.0f;
    bool enabled = true;
};

struct DirLightGPU
{
    glm::vec4 direction;   // xyz + pad
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

struct PointLight {
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float intensity = 1.0f;
    bool enabled = true;
};

struct PointLightGPU
{
    glm::vec4 position;    // xyz

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    glm::vec4 params;      
    // x = constant
    // y = linear
    // z = quadratic
    // w = intensity
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

    float intensity = 1.0f;
    bool enabled = true;
};

struct SpotLightGPU
{
    glm::vec4 position;
    glm::vec4 direction;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    glm::vec4 params1; 
    // x = cutOff
    // y = outerCutOff
    // z = constant
    // w = linear

    glm::vec4 params2;
    // x = quadratic
    // y = intensity
};

// =======================
// Light Manager
// =======================
struct LightManager
{
    DirLight dirLight;
    std::vector<PointLight> pointLights;
    SpotLight spotLight;

    // ========= GPU cache =========
    DirLightGPU dirLightGPU;
    std::vector<PointLightGPU> pointLightsGPU;
    SpotLightGPU spotLightGPU;
    // dirty flags for incremental updates
    bool dirDirty = false;
    bool spotDirty = false;
    std::vector<char> pointLightDirty; // 0/1 per point light
};

// =======================
// Object（for debug / light visual）
// =======================
struct Object
{
    Mesh* mesh = nullptr;

    glm::vec3 position{0.0f};
    glm::vec3 scale{1.0f};

    bool isEmissive = false;
    glm::vec3 emissiveColor{0.0f};

    glm::mat4 getModel() const
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        return model;
    }

};


// =======================
// RenderGroup (GPU batch核心)
// =======================
struct RenderGroup
{
    Mesh* mesh = nullptr;
    std::vector<glm::mat4> models;

    bool isEmissive = false;
    glm::vec3 emissiveColor = glm::vec3(0.0f);

    void* material = nullptr;
    bool instanceDirty = true;

    void setModels(const std::vector<glm::mat4>& m)
    {
        models = m;
        instanceDirty = true;
    }
};

// =======================
// Scene
// =======================
class Scene {
public:
    std::vector<RenderGroup> renderGroups;
    std::vector<Object> lightVisuals;

    LightManager lights;

    void update(float dt);
};

#endif


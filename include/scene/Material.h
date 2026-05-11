#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

#include "graphics/Texture.h"

// ============================================
// Material
// Pure GPU renderer material data
// ============================================

struct Material
{
    // ========================================
    // Textures
    Texture* diffuse = nullptr;

    Texture* specular = nullptr;

    // ========================================
    // Surface properties
    float shininess = 32.0f;

    float metallic = 0.0f;

    float roughness = 1.0f;

    // ========================================
    // Emissive
    glm::vec3 emissive = glm::vec3(0.0f);

    // ========================================
    // Rendering flags
    bool transparent = false;

    bool doubleSided = false;

    // ========================================
    // Constructors
    Material() = default;

    Material(
        Texture* diffuseTex,
        Texture* specularTex,
        float shiny = 32.0f)
    {
        diffuse = diffuseTex;
        specular = specularTex;
        shininess = shiny;
    }
};

#endif // MATERIAL_H
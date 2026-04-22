#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"
#include "Shader.h"
#include <glm/glm.hpp>

struct Material {
    Texture* diffuse = nullptr;
    Texture* specular = nullptr;
    float shininess = 32.0f;

    Material() = default;
    Material(Texture* d, Texture* s, float sh = 32.0f)
        : diffuse(d), specular(s), shininess(sh) {}

    void bind(const Shader& shader) const {
        if (diffuse) diffuse->bind(0);
        if (specular) specular->bind(1);
        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", shininess);
    }
};

#endif // MATERIAL_H

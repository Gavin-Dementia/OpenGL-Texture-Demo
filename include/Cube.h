#ifndef CUBE_H
#define CUBE_H

#include <glad/glad.h>
#include "Mesh.h"

class Cube : public Mesh
{
public:
    Cube();
    ~Cube() override;

    void draw() override;
    void drawInstanced(int count) override;
};

#endif

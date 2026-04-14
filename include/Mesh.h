#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

class Mesh
{
public:
    Mesh() = default;
    virtual ~Mesh()= default;
    virtual void draw() = 0;

protected:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
};

#endif


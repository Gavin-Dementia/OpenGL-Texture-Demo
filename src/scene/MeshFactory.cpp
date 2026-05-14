#include "scene/MeshFactory.h"

Mesh MeshFactory::createCube()
{
    Mesh mesh;

    mesh.vertices =
    {
        {{-0.5f,-0.5f, 0.5f}, {0,0,1}, {0,0}},
        {{ 0.5f,-0.5f, 0.5f}, {0,0,1}, {1,0}},
        {{ 0.5f, 0.5f, 0.5f}, {0,0,1}, {1,1}},
        {{-0.5f, 0.5f, 0.5f}, {0,0,1}, {0,1}},

        {{-0.5f,-0.5f,-0.5f}, {0,0,-1}, {0,0}},
        {{ 0.5f,-0.5f,-0.5f}, {0,0,-1}, {1,0}},
        {{ 0.5f, 0.5f,-0.5f}, {0,0,-1}, {1,1}},
        {{-0.5f, 0.5f,-0.5f}, {0,0,-1}, {0,1}},
    };

    mesh.indices =
    {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        4,0,3, 3,7,4,
        1,5,6, 6,2,1,
        3,2,6, 6,7,3,
        4,5,1, 1,0,4
    };

    mesh.indexCount =
        static_cast<uint32_t>(mesh.indices.size());

    mesh.computeBounds();

    return mesh;
}


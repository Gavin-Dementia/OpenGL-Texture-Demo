#ifndef MESH_BUFFER_H
#define MESH_BUFFER_H

#include <vector>

#include "gpu/buffers/SSBO.h"
#include "gpu/layout/Vertex.h"
#include "gpu/layout/MeshDrawData.h"
#include "scene/Mesh.h"

struct GPUMeshData
{
    uint32_t indexOffset;
    uint32_t vertexOffset;
    uint32_t indexCount;
    uint32_t vertexCount;
};

class MeshBuffer
{
public:

    void init();
    // uint32_t uploadMesh(const std::vector<Vertex>& vertices,
    //                     const std::vector<uint32_t>& indices);

    MeshDrawData uploadMesh(const Mesh& mesh);

    const GPUMeshData& getMesh(uint32_t meshID) const;

    void bind() const;

private:

    SSBO vertexBuffer;
    SSBO indexBuffer;

    std::vector<GPUMeshData> meshTable;

    uint32_t vertexCursor = 0;
    uint32_t indexCursor = 0;
    uint32_t vertexOffset = 0;
    uint32_t indexOffset = 0;
};

#endif  // MESH_BUFFER_H


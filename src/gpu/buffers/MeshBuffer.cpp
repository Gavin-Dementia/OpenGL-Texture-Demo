#include "gpu/buffers/MeshBuffer.h"
#include "gpu/layout/Binding.h"
#include <glad/glad.h>
#include <cstring>
#include <stdexcept>

void MeshBuffer::init()
{
    vertexBuffer.create(1024 * 1024 * sizeof(Vertex), GL_DYNAMIC_DRAW);
    indexBuffer.create(1024 * 1024 * sizeof(uint32_t), GL_DYNAMIC_DRAW);

    vertexOffset = 0;
    indexOffset = 0;

    meshTable.clear();
}

// uint32_t MeshBuffer::uploadMesh(
//     const std::vector<Vertex>& vertices,
//     const std::vector<uint32_t>& indices)
// {
//     uint32_t meshID = static_cast<uint32_t>(meshTable.size());

//     GPUMeshData mesh;
//     mesh.vertexOffset = vertexCursor;
//     mesh.indexOffset  = indexCursor;
//     mesh.vertexCount  = static_cast<uint32_t>(vertices.size());
//     mesh.indexCount   = static_cast<uint32_t>(indices.size());

//     // =========================
//     // upload vertex data
//     vertexBuffer.upload(
//         vertices.data(),
//         vertices.size() * sizeof(Vertex),
//         vertexCursor * sizeof(Vertex)
//     );

//     // =========================
//     // upload index data
//     indexBuffer.upload(
//         indices.data(),
//         indices.size() * sizeof(uint32_t),
//         indexCursor * sizeof(uint32_t)
//     );

//     // =========================
//     // advance cursor
//     vertexCursor += mesh.vertexCount;
//     indexCursor  += mesh.indexCount;

//     meshTable.push_back(mesh);

//     return meshID;
// }

MeshDrawData MeshBuffer::uploadMesh(const Mesh& mesh)
{
    MeshDrawData draw{};

    draw.vertexOffset = vertexOffset;
    draw.indexOffset  = indexOffset;

    draw.vertexCount =
        static_cast<uint32_t>(mesh.vertices.size());

    draw.indexCount =
        static_cast<uint32_t>(mesh.indices.size());

    vertexBuffer.upload(
        mesh.vertices.data(),
        mesh.vertices.size() * sizeof(Vertex),
        vertexOffset * sizeof(Vertex));

    indexBuffer.upload(
        mesh.indices.data(),
        mesh.indices.size() * sizeof(uint32_t),
        indexOffset * sizeof(uint32_t));

    vertexOffset += draw.vertexCount;
    indexOffset  += draw.indexCount;

    return draw;
}

const GPUMeshData& MeshBuffer::getMesh(uint32_t meshID) const
{
    if (meshID >= meshTable.size())
        throw std::runtime_error("MeshBuffer: invalid meshID");

    return meshTable[meshID];
}

void MeshBuffer::bind() const
{
    vertexBuffer.bindBase(Binding::MeshBoundsBuffer); 
    indexBuffer.bindBase(Binding::InstanceBuffer);
}


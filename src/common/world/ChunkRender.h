//
// Created by reili on 2020/9/27.
//

#ifndef LEARNCRAFT_CHUNKRENDER_H
#define LEARNCRAFT_CHUNKRENDER_H

#include <array>

#include "common/types.h"
#include "Chunk.h"

//struct Vertex
//{
//    x,y,z; 4+4+4 = 12;
//    3;  // normal index
//    2;  // uv index
//    15; // texture index
//};

using vertex_t = u32;
using index_t = u32;

struct ChunkMesh
{
    std::vector<vertex_t> vertices;
    std::vector<index_t> indices;
};

struct MeshFace
{
    std::array<u8, 12> vertices;
    u8 normal; // +x:0 +y:1 +z:2 -x:3 -y:4 -z:5
};

struct MeshFaces
{
    static constexpr MeshFace kFrontFace {
            {1, 1, 1,
             0, 1, 1,
             0, 0, 1,
             1, 0, 1},
            2};

    static constexpr MeshFace kLeftFace {
            {0, 1, 1,
             0, 1, 0,
             0, 0, 0,
             0, 0, 1},
            3};
    static constexpr MeshFace kBackFace = {
            {0, 1, 0,
             1, 1, 0,
             1, 0, 0,
             0, 0, 0},
            5};

    static constexpr MeshFace kRightFace = {
            {1, 1, 0,
             1, 1, 1,
             1, 0, 1,
             1, 0, 0},
            0};

    static constexpr MeshFace kTopFace = {
            {1, 1, 0,
             0, 1, 0,
             0, 1, 1,
             1, 1, 1},
            1};

    static constexpr MeshFace kBottomFace = {
            {0, 0, 0,
             1, 0, 0,
             1, 0, 1,
             0, 0, 1},
            4};
};

struct ChunkBuffGL
{
    void CreateBuff(const std::vector<vertex_t> &vertices, const std::vector<index_t> &indices)
    {
        index_count = indices.size();
        // 顶点数组对象, 顶点缓冲对象, 索引缓冲对象
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        // 绑定
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
        constexpr GLuint location = 0;
        constexpr GLint floatCount = 1;
        glVertexAttribIPointer(location, floatCount, GL_UNSIGNED_INT, 0, (GLvoid*)0);
        glEnableVertexAttribArray(location);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);
    }

    void draw()
    {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    }

    GLuint vao, vbo, ebo;
    std::size_t index_count;
};

struct ChunkMeshUtils
{
    static void add_face(ChunkMesh &mesh, const MeshFace &face, const VoxelIndex &voxelIndex, tex_t texture)
    {
        std::size_t index = 0;
        for(std::size_t i = 0; i < 4; ++i)
        {
            u8 x = face.vertices[index++] + voxelIndex.x;
            u8 y = face.vertices[index++] + voxelIndex.y;
            u8 z = face.vertices[index++] + voxelIndex.z;

            u32 vertex =
                    x | y << 4 | z << 8 | face.normal << 12 | i << 15 | texture << 17;

            mesh.vertices.push_back(vertex);
        }

        auto index_start = mesh.vertices.size() - 4;
        mesh.indices.push_back(index_start);
        mesh.indices.push_back(index_start + 1);
        mesh.indices.push_back(index_start + 2);
        mesh.indices.push_back(index_start + 2);
        mesh.indices.push_back(index_start + 3);
        mesh.indices.push_back(index_start);
    }

    static ChunkMesh* CreateChunkMesh(World &world, const Chunk &chunk)
    {
        ChunkMesh *mesh = new ChunkMesh;
        for (int x = 0; x < WorldConfig::kChunkSizeX; ++x)
        {
            for (int y = 0; y < WorldConfig::kChunkSizeY; ++y)
            {
                for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z)
                {
                    auto voxel_index = VoxelIndex{ x, y, z };

                    auto voxel = chunk.GetVoxel(voxel_index);

                    if(voxel == static_cast<voxel_t>(CommonVoxel::Air))
                        continue;

                    if (!world.IsVoxelTypeSolidUnbound(chunk.GetChunkIndex(), UnboundVoxelIndex(x, y, z + 1)))
                    {
                        add_face(*mesh, MeshFaces::kFrontFace, voxel_index, world.GetVoxelTextureLayer(voxel, CubeSide::Front));
                    }

                    if (!world.IsVoxelTypeSolidUnbound(chunk.GetChunkIndex(), UnboundVoxelIndex(x, y, z - 1)))
                    {
                        add_face(*mesh, MeshFaces::kBackFace, voxel_index, world.GetVoxelTextureLayer(voxel, CubeSide::Back));
                    }

                    if (!world.IsVoxelTypeSolidUnbound(chunk.GetChunkIndex(), UnboundVoxelIndex(x, y - 1, z)))
                    {
                        add_face(*mesh, MeshFaces::kBottomFace, voxel_index, world.GetVoxelTextureLayer(voxel, CubeSide::Bottom));
                    }

                    if (!world.IsVoxelTypeSolidUnbound(chunk.GetChunkIndex(), UnboundVoxelIndex(x, y + 1, z)))
                    {
                        add_face(*mesh, MeshFaces::kTopFace, voxel_index, world.GetVoxelTextureLayer(voxel, CubeSide::Top));
                    }

                    if (!world.IsVoxelTypeSolidUnbound(chunk.GetChunkIndex(), UnboundVoxelIndex(x - 1, y, z)))
                    {
                        add_face(*mesh, MeshFaces::kLeftFace, voxel_index, world.GetVoxelTextureLayer(voxel, CubeSide::Left));
                    }

                    if (!world.IsVoxelTypeSolidUnbound(chunk.GetChunkIndex(), UnboundVoxelIndex(x + 1, y, z)))
                    {
                        add_face(*mesh, MeshFaces::kRightFace, voxel_index, world.GetVoxelTextureLayer(voxel, CubeSide::Right));
                    }
                }
            }
        }
        return mesh;
    }
};


#endif //LEARNCRAFT_CHUNKRENDER_H

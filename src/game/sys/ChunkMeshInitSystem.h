//
// Created by reili on 2020/10/7.
//

#ifndef LEARNCRAFT_CHUNKMESHINITSYSTEM_H
#define LEARNCRAFT_CHUNKMESHINITSYSTEM_H

#include <entt/entt.hpp>
#include <game/comp/chunk_index.h>
#include <game/comp/VoxelData.h>
#include <game/comp/ChunkVoxelEntities.h>
#include <game/comp/ChunkMeshInit.h>
#include <game/comp/RuntimeVoxelTextureLayer.h>
#include <common/world/WorldUtils.h>
#include <common/world/ChunkRender.h>
#include <game/comp/render.h>

// create chunk vertices and indices
struct ChunkMeshInitSystem {
    static void Tick(entt::registry &registry, std::unordered_map<ChunkIndex, entt::entity, hash_tuple> &chunk_entity_map){
        auto view = registry.view<ChunkIndexComponent, ChunkVoxelEntitiesComponent, ChunkMeshInitComponent>();
        auto voxel_view = registry.view<VoxelDataComponent, RuntimeVoxelTextureLayerComponent>();
        for(auto entity : view)
        {
            auto &chunk_index = view.get<ChunkIndexComponent>(entity);
            auto &voxel_entities = view.get<ChunkVoxelEntitiesComponent>(entity);

            std::array<voxel_traits_t, WorldConfig::ChunkSize()> voxels_traits;
            std::array<VoxelTextureLayers, WorldConfig::ChunkSize()> textures;

            for (int y = 0; y < WorldConfig::kChunkSizeY; ++y) {
                for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z) {
                    for (int x = 0; x < WorldConfig::kChunkSizeX; ++x) {
                        auto array_index = WorldUtils::voxel_index_to_data_index({x, y, z});
                        auto voxel_entity = voxel_entities.voxels[array_index];
                        auto &voxel_data = voxel_view.get<VoxelDataComponent>(voxel_entity);
                        auto &layers = voxel_view.get<RuntimeVoxelTextureLayerComponent>(voxel_entity);
                        voxels_traits[array_index] = voxel_data.traits;
                        textures[array_index] = layers.layers;
                    }
                }
            }

            auto mesh = CreateChunkMeshWithoutBorder(voxels_traits, textures);

            {
                auto [chunk_index_x, chunk_index_y, chunk_index_z] = chunk_index.val;
                // top border
                ChunkIndex top_chunk_index = ChunkIndex{chunk_index_x, chunk_index_y+1, chunk_index_z};
                if(auto another_chunk_find = chunk_entity_map.find(top_chunk_index); another_chunk_find != chunk_entity_map.end())
                {
                    auto &another_voxel_entities = view.get<ChunkVoxelEntitiesComponent>(another_chunk_find->second);
                    for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z) {
                        for (int x = 0; x < WorldConfig::kChunkSizeX; ++x) {
                            auto voxel_index = VoxelIndex{x, WorldConfig::kChunkSizeY-1, z};
                            // Get data index
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            // Get above voxel data
                            auto another_array_index = WorldUtils::voxel_index_to_data_index({x, 0, z});
                            auto another_voxel_entity = another_voxel_entities.voxels[another_array_index];
                            auto &another_voxel_data = voxel_view.get<VoxelDataComponent>(another_voxel_entity);

                            if(!has_trait_opaque(another_voxel_data.traits))
                            {
                                ChunkMeshUtils::add_face(mesh, MeshFaces::kTopFace, voxel_index, textures[array_index].top);
                            }
                        }
                    }
                }
                else
                {
                    for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z) {
                        for (int x = 0; x < WorldConfig::kChunkSizeX; ++x) {
                            auto voxel_index = VoxelIndex{x, WorldConfig::kChunkSizeY-1, z};
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            ChunkMeshUtils::add_face(mesh, MeshFaces::kTopFace, voxel_index, textures[array_index].top);
                        }
                    }
                }

                // bottom border
                ChunkIndex bottom_chunk_index = ChunkIndex{chunk_index_x, chunk_index_y-1, chunk_index_z};
                if(auto another_chunk_find = chunk_entity_map.find(bottom_chunk_index); another_chunk_find != chunk_entity_map.end())
                {
                    auto &another_voxel_entities = view.get<ChunkVoxelEntitiesComponent>(another_chunk_find->second);
                    for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z) {
                        for (int x = 0; x < WorldConfig::kChunkSizeX; ++x) {
                            auto voxel_index = VoxelIndex{x, 0, z};
                            // Get data index
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            // Get bellow voxel data
                            auto another_array_index = WorldUtils::voxel_index_to_data_index({x, WorldConfig::kChunkSizeY-1, z});
                            auto another_voxel_entity = another_voxel_entities.voxels[another_array_index];
                            auto &another_voxel_data = voxel_view.get<VoxelDataComponent>(another_voxel_entity);

                            if(!has_trait_opaque(another_voxel_data.traits))
                            {
                                ChunkMeshUtils::add_face(mesh, MeshFaces::kBottomFace, voxel_index, textures[array_index].bottom);
                            }
                        }
                    }
                }
                else
                {
                    for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z) {
                        for (int x = 0; x < WorldConfig::kChunkSizeX; ++x) {
                            auto voxel_index = VoxelIndex{x, 0, z};
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            ChunkMeshUtils::add_face(mesh, MeshFaces::kBottomFace, voxel_index, textures[array_index].bottom);
                        }
                    }
                }

                // left border
                ChunkIndex left_chunk_index = ChunkIndex{chunk_index_x-1, chunk_index_y, chunk_index_z};
                if(auto another_chunk_find = chunk_entity_map.find(left_chunk_index); another_chunk_find != chunk_entity_map.end())
                {
                    auto &another_voxel_entities = view.get<ChunkVoxelEntitiesComponent>(another_chunk_find->second);
                    for (int y = 0; y < WorldConfig::kChunkSizeY; ++y) {
                        for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z) {
                            auto voxel_index = VoxelIndex{0, y, z};
                            // Get data index
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            // Get left voxel data
                            auto another_array_index = WorldUtils::voxel_index_to_data_index({WorldConfig::kChunkSizeX-1, y, z});
                            auto another_voxel_entity = another_voxel_entities.voxels[another_array_index];
                            auto &another_voxel_data = voxel_view.get<VoxelDataComponent>(another_voxel_entity);

                            if(!has_trait_opaque(another_voxel_data.traits))
                            {
                                ChunkMeshUtils::add_face(mesh, MeshFaces::kLeftFace, voxel_index, textures[array_index].left);
                            }
                        }
                    }
                }
                else
                {
                    for (int y = 0; y < WorldConfig::kChunkSizeY; ++y) {
                        for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z) {
                            auto voxel_index = VoxelIndex{0, y, z};
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            ChunkMeshUtils::add_face(mesh, MeshFaces::kLeftFace, voxel_index, textures[array_index].left);
                        }
                    }
                }

                // right border
                ChunkIndex right_chunk_index = ChunkIndex{chunk_index_x+1, chunk_index_y, chunk_index_z};
                if(auto another_chunk_find = chunk_entity_map.find(right_chunk_index); another_chunk_find != chunk_entity_map.end())
                {
                    auto &another_voxel_entities = view.get<ChunkVoxelEntitiesComponent>(another_chunk_find->second);
                    for (int y = 0; y < WorldConfig::kChunkSizeY; ++y) {
                        for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z) {
                            auto voxel_index = VoxelIndex{WorldConfig::kChunkSizeX-1, y, z};
                            // Get data index
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            // Get right voxel data
                            auto another_array_index = WorldUtils::voxel_index_to_data_index({0, y, z});
                            auto another_voxel_entity = another_voxel_entities.voxels[another_array_index];
                            auto &another_voxel_data = voxel_view.get<VoxelDataComponent>(another_voxel_entity);

                            if(!has_trait_opaque(another_voxel_data.traits))
                            {
                                ChunkMeshUtils::add_face(mesh, MeshFaces::kRightFace, voxel_index, textures[array_index].right);
                            }
                        }
                    }
                }
                else
                {
                    for (int y = 0; y < WorldConfig::kChunkSizeY; ++y) {
                        for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z) {
                            auto voxel_index = VoxelIndex{WorldConfig::kChunkSizeX-1, y, z};
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            ChunkMeshUtils::add_face(mesh, MeshFaces::kRightFace, voxel_index, textures[array_index].right);
                        }
                    }
                }

                // front border
                ChunkIndex front_chunk_index = ChunkIndex{chunk_index_x, chunk_index_y, chunk_index_z+1};
                if(auto another_chunk_find = chunk_entity_map.find(front_chunk_index); another_chunk_find != chunk_entity_map.end())
                {
                    auto &another_voxel_entities = view.get<ChunkVoxelEntitiesComponent>(another_chunk_find->second);
                    for (int y = 0; y < WorldConfig::kChunkSizeY; ++y) {
                        for (int x = 0; x < WorldConfig::kChunkSizeZ; ++x) {
                            auto voxel_index = VoxelIndex{x, y, WorldConfig::kChunkSizeZ-1};
                            // Get data index
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            // Get front voxel data
                            auto another_array_index = WorldUtils::voxel_index_to_data_index({x, y, 0});
                            auto another_voxel_entity = another_voxel_entities.voxels[another_array_index];
                            auto &another_voxel_data = voxel_view.get<VoxelDataComponent>(another_voxel_entity);

                            if(!has_trait_opaque(another_voxel_data.traits))
                            {
                                ChunkMeshUtils::add_face(mesh, MeshFaces::kFrontFace, voxel_index, textures[array_index].front);
                            }
                        }
                    }
                }
                else
                {
                    for (int y = 0; y < WorldConfig::kChunkSizeY; ++y) {
                        for (int x = 0; x < WorldConfig::kChunkSizeZ; ++x) {
                            auto voxel_index = VoxelIndex{x, y, WorldConfig::kChunkSizeZ-1};
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            ChunkMeshUtils::add_face(mesh, MeshFaces::kFrontFace, voxel_index, textures[array_index].front);
                        }
                    }
                }

                // back border
                ChunkIndex back_chunk_index = ChunkIndex{chunk_index_x, chunk_index_y, chunk_index_z-1};
                if(auto another_chunk_find = chunk_entity_map.find(back_chunk_index); another_chunk_find != chunk_entity_map.end())
                {
                    auto &another_voxel_entities = view.get<ChunkVoxelEntitiesComponent>(another_chunk_find->second);
                    for (int y = 0; y < WorldConfig::kChunkSizeY; ++y) {
                        for (int x = 0; x < WorldConfig::kChunkSizeZ; ++x) {
                            auto voxel_index = VoxelIndex{x, y, 0};
                            // Get data index
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            // Get back voxel data
                            auto another_array_index = WorldUtils::voxel_index_to_data_index({x, y, WorldConfig::kChunkSizeZ-1});
                            auto another_voxel_entity = another_voxel_entities.voxels[another_array_index];
                            auto &another_voxel_data = voxel_view.get<VoxelDataComponent>(another_voxel_entity);

                            if(!has_trait_opaque(another_voxel_data.traits))
                            {
                                ChunkMeshUtils::add_face(mesh, MeshFaces::kBackFace, voxel_index, textures[array_index].back);
                            }
                        }
                    }
                }
                else
                {
                    for (int y = 0; y < WorldConfig::kChunkSizeY; ++y) {
                        for (int x = 0; x < WorldConfig::kChunkSizeZ; ++x) {
                            auto voxel_index = VoxelIndex{x, y, 0};
                            auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);
                            auto voxel_traits = voxels_traits[array_index];
                            if(has_trait_empty(voxel_traits))
                                continue;

                            ChunkMeshUtils::add_face(mesh, MeshFaces::kBackFace, voxel_index, textures[array_index].back);
                        }
                    }
                }
            }

            if(mesh.vertices.size() > 0)
            {
                auto vao = CreateBuff(mesh.vertices, mesh.indices);
                registry.emplace<ChunkRenderComponent>(entity, vao, mesh.indices.size());
            }
        }

        auto view2 = registry.view<ChunkMeshInitComponent>();
        for(auto entity : view2)
        {
            registry.remove<ChunkMeshInitComponent>(entity);
        }
    }

    // avoid data race
    static ChunkMesh CreateChunkMeshWithoutBorder(const std::array<voxel_traits_t , WorldConfig::ChunkSize()> &voxels_traits, const std::array<VoxelTextureLayers, WorldConfig::ChunkSize()> &textures)
    {
        ChunkMesh mesh;
        // avoid border
        for (int y = 0; y < WorldConfig::kChunkSizeY; ++y)
        {
            for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z)
            {
                for (int x = 0; x < WorldConfig::kChunkSizeX; ++x)
                {
                    auto voxel_index = VoxelIndex{ x, y, z };
                    auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);

                    auto voxel_traits = voxels_traits[array_index];

                    if(has_trait_empty(voxel_traits))
                        continue;

                    if(z + 1 != WorldConfig::kChunkSizeZ){
                        if(auto front_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x, y, z+1))]; !has_trait_opaque(front_traits))
                        {
                            ChunkMeshUtils::add_face(mesh, MeshFaces::kFrontFace, voxel_index, textures[array_index].front);
                        }
                    }

                    if(z != 0){
                        if(auto back_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x, y, z-1))]; !has_trait_opaque(back_traits))
                        {
                            ChunkMeshUtils::add_face(mesh, MeshFaces::kBackFace, voxel_index, textures[array_index].back);
                        }
                    }

                    if(y != 0){
                        if(auto bottom_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x, y-1, z))]; !has_trait_opaque(bottom_traits))
                        {
                            ChunkMeshUtils::add_face(mesh, MeshFaces::kBottomFace, voxel_index, textures[array_index].bottom);
                        }
                    }

                    if(y + 1 != WorldConfig::kChunkSizeY){
                        if(auto top_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x, y+1, z))]; !has_trait_opaque(top_traits))
                        {
                            ChunkMeshUtils::add_face(mesh, MeshFaces::kTopFace, voxel_index, textures[array_index].top);
                        }
                    }

                    if(x != 0){
                        if(auto left_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x-1, y, z))]; !has_trait_opaque(left_traits))
                        {
                            ChunkMeshUtils::add_face(mesh, MeshFaces::kLeftFace, voxel_index, textures[array_index].left);
                        }
                    }

                    if(x + 1 != WorldConfig::kChunkSizeX){
                        if(auto right_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x+1, y, z))]; !has_trait_opaque(right_traits))
                        {
                            ChunkMeshUtils::add_face(mesh, MeshFaces::kRightFace, voxel_index, textures[array_index].right);
                        }
                    }
                }
            }
        }
        return mesh;
    }

    static GLuint CreateBuff(const std::vector<vertex_t> &vertices, const std::vector<index_t> &indices)
    {
        // 顶点数组对象, 顶点缓冲对象, 索引缓冲对象
        GLuint vao, vbo, ebo;
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

        return vao;
    }
};

#endif //LEARNCRAFT_CHUNKMESHINITSYSTEM_H

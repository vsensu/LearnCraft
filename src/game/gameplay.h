//
// Created by reili on 2020/9/30.
//

#ifndef LEARNCRAFT_GAMEPLAY_H
#define LEARNCRAFT_GAMEPLAY_H

#include <entt/entt.hpp>
#include <glad/glad.h>
#include <functional>
#include <array>
#include <game/comp/chunk_index.h>

#include "common/world/WorldUtils.h"
#include "common/world/WorldTypes.h"

#include "common/Shader.hpp"

/*
 * define ENTT_ID_TYPE std::uint32_t
 * ===================================
 * Config(Type) is comfirmed instance.
 * ===================================
 *
 * ============================
 * tables
 * ============================
 *
 * Entity Table
 * ------------------------------------------------
 * id   |   component_inst_ids(vector)
 * ------------------------------------------------
 *
 * Component Table
 * --------------------------------------------------------------
 * component_inst_id    |   class_reflection  |   serialized_data(str)
 * --------------------------------------------------------------
 * default_1
 * default_2
 * ...
 * inst_1
 * inst_2
 *
 * ===========================
 * Generated Data
 * ===========================
 * Chunk Mesh
 * Chunk to Create
 * */

struct ChunkInitComponent
{
    ChunkIndex chunk_index;
};

struct ChunkVoxelEntitiesComponent
{
    std::array<ENTT_ID_TYPE , WorldConfig::ChunkSize()> voxels;
};

struct RuntimeVoxelTextureLayerComponent
{
    VoxelTextureLayers layers;
};

using voxel_traits_t = u32;

struct VoxelDataComponent
{
    name_t name;
    voxel_traits_t traits;
};

enum VoxelTrait: voxel_traits_t
{
    Empty = 1,
    Opaque = 1u << 1,
};

bool has_trait_empty(voxel_traits_t traits)
{
    return traits & VoxelTrait::Empty;
}

bool has_trait_opaque(voxel_traits_t traits)
{
    return traits & (VoxelTrait::Opaque);
}

struct VoxelTexturePathComponent
{
    VoxelTextureNames textures;
};

struct ChunkMeshInitComponent
{
    enum class State
    {
        Create,
        Destroy,
        Update,
    };
};

std::unordered_map<ChunkIndex, ENTT_ID_TYPE, hash_tuple> chunk_entity_map;
std::array<tex_t, (1u << 16)> texture_layers;
TextureManager texture_manager;

enum CoreEntity: ENTT_ID_TYPE
{
    // Use entity_id not voxel_t
    Block_Start = 0,
    Block_Empty = Block_Start + 1,
    Block_Dirt = Block_Start + 2,
    Block_Grass = Block_Start + 3,
    Block_Stone = Block_Start + 4,
    // ...
};

void InitGameCore(entt::registry &registry)
{
    auto empty = registry.create(static_cast<const entt::entity>(CoreEntity::Block_Empty));
    auto dirt = registry.create(static_cast<const entt::entity>(CoreEntity::Block_Dirt));
    auto grass = registry.create(static_cast<const entt::entity>(CoreEntity::Block_Grass));
    auto stone = registry.create(static_cast<const entt::entity>(CoreEntity::Block_Stone));

    registry.emplace<VoxelDataComponent>(empty, "Empty", VoxelTrait::Empty);
    registry.emplace<VoxelDataComponent>(dirt, "Dirt", VoxelTrait::Empty | VoxelTrait::Opaque);
    registry.emplace<VoxelDataComponent>(grass, "Grass", VoxelTrait::Empty | VoxelTrait::Opaque);
    registry.emplace<VoxelDataComponent>(stone, "Stone", VoxelTrait::Empty | VoxelTrait::Opaque);

    // Convert config to runtime
    auto dirt_layer = texture_manager.GetVoxelTextureLayer("dirt");
    registry.emplace<RuntimeVoxelTextureLayerComponent>(dirt,
                                                        VoxelTextureLayers{
                                                                .front = dirt_layer, .back = dirt_layer,
                                                                .left = dirt_layer, .right = dirt_layer,
                                                                .top = dirt_layer, .bottom = dirt_layer,
                                                        });
    registry.emplace<RuntimeVoxelTextureLayerComponent>(grass, VoxelTextureLayers{
            .front = dirt_layer, .back = dirt_layer,
            .left = dirt_layer, .right = dirt_layer,
            .top = texture_manager.GetVoxelTextureLayer("grass_top"),
            .bottom = dirt_layer,
    });
    auto stone_layer = texture_manager.GetVoxelTextureLayer("stone");
    registry.emplace<RuntimeVoxelTextureLayerComponent>(stone,
                                                        VoxelTextureLayers{.bottom = stone_layer, .top = stone_layer,
                                                                .front = stone_layer, .back = stone_layer,
                                                                .left = stone_layer, .right = stone_layer});
}

ENTT_ID_TYPE get_chunk_entity(const ChunkIndex &chunkIndex)
{
    return chunk_entity_map[chunkIndex];
}

// create chunk vertices and indices
struct ChunkMeshInitSystem {
    static void Tick(const World &world, entt::registry &registry){
        auto view = registry.view<ChunkIndexComponent, ChunkVoxelEntitiesComponent, ChunkMeshInitComponent>();
        for(auto entity : view)
        {
            auto &chunk_index = view.get<ChunkIndexComponent>(entity);
            auto &voxel_entities = view.get<ChunkVoxelEntitiesComponent>(entity);

            std::array<voxel_traits_t, WorldConfig::ChunkSize()> voxels_traits;
            std::array<VoxelTextureLayers, WorldConfig::ChunkSize()> textures;

            auto voxel_view = registry.view<VoxelDataComponent, RuntimeVoxelTextureLayerComponent>();
            for (int x = 0; x < WorldConfig::kChunkSizeX; ++x)
            {
                for (int y = 0; y < WorldConfig::kChunkSizeY; ++y)
                {
                    for (int z = 0; z < WorldConfig::kChunkSizeZ; ++z) {
                        auto array_index = WorldUtils::voxel_index_to_data_index({x, y, z});
                        auto &voxel_data = voxel_view.get<VoxelDataComponent>(
                                static_cast<const entt::entity>(voxel_entities.voxels[array_index]));
                        auto &layers = voxel_view.get<RuntimeVoxelTextureLayerComponent>(
                                static_cast<const entt::entity>(voxel_entities.voxels[array_index]));
                        voxels_traits[array_index] = voxel_data.traits;
                        textures[array_index] = layers.layers;
                    }
                }
            }

            CreateChunkMeshWithoutBorder(voxels_traits, textures);

            registry.remove<ChunkMeshInitComponent>(entity);
        }
    }

    // avoid data race
    static ChunkMesh CreateChunkMeshWithoutBorder(const std::array<voxel_traits_t , WorldConfig::ChunkSize()> &voxels_traits, const std::array<VoxelTextureLayers, WorldConfig::ChunkSize()> &textures)
    {
        ChunkMesh mesh;
        // avoid border
        for (int x = 1; x + 1 < WorldConfig::kChunkSizeX; ++x)
        {
            for (int y = 1; y + 1 < WorldConfig::kChunkSizeY; ++y)
            {
                for (int z = 1; z + 1 < WorldConfig::kChunkSizeZ; ++z)
                {
                    auto voxel_index = VoxelIndex{ x, y, z };
                    auto array_index = WorldUtils::voxel_index_to_data_index(voxel_index);

                    auto voxel_traits = voxels_traits[array_index];

                    if(has_trait_empty(voxel_traits))
                        continue;

                    if(auto front_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x, y, z+1))]; !has_trait_opaque(front_traits))
                    {
                        ChunkMeshUtils::add_face(mesh, MeshFaces::kFrontFace, voxel_index, textures[array_index].front);
                    }

                    if(auto back_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x, y, z-1))]; !has_trait_opaque(back_traits))
                    {
                        ChunkMeshUtils::add_face(mesh, MeshFaces::kBackFace, voxel_index, textures[array_index].back);
                    }

                    if(auto bottom_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x, y-1, z))]; !has_trait_opaque(bottom_traits))
                    {
                        ChunkMeshUtils::add_face(mesh, MeshFaces::kBottomFace, voxel_index, textures[array_index].bottom);
                    }

                    if(auto top_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x, y+1, z))]; !has_trait_opaque(top_traits))
                    {
                        ChunkMeshUtils::add_face(mesh, MeshFaces::kTopFace, voxel_index, textures[array_index].top);
                    }

                    if(auto left_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x-1, y, z))]; !has_trait_opaque(left_traits))
                    {
                        ChunkMeshUtils::add_face(mesh, MeshFaces::kLeftFace, voxel_index, textures[array_index].left);
                    }

                    if(auto right_traits = voxels_traits[WorldUtils::voxel_index_to_data_index(VoxelIndex(x+1, y, z))]; !has_trait_opaque(right_traits))
                    {
                        ChunkMeshUtils::add_face(mesh, MeshFaces::kRightFace, voxel_index, textures[array_index].right);
                    }
                }
            }
        }
        return mesh;
    }
};

struct ChunkInitSystem{
    static void Tick(std::function<std::array<voxel_t, WorldConfig::ChunkSize()>(const Position &)> generator,
                      entt::registry &registry) {
        auto view = registry.view<ChunkInitComponent>();
        for (auto entity : view) {
            auto &chunk_init = view.get<ChunkInitComponent>(entity);

            registry.emplace<ChunkIndexComponent>(entity, chunk_init.chunk_index);
            registry.emplace<ChunkVoxelEntitiesComponent>(entity, generator(WorldUtils::ChunkIndexToPosition(chunk_init.chunk_index)));

            registry.remove<ChunkInitComponent>(entity);
        }
    }
};


#endif //LEARNCRAFT_GAMEPLAY_H

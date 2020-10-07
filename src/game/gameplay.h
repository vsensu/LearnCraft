//
// Created by reili on 2020/9/30.
//

#ifndef LEARNCRAFT_GAMEPLAY_H
#define LEARNCRAFT_GAMEPLAY_H

#include <string>
#include <entt/entt.hpp>
#include <glad/glad.h>
#include <functional>
#include <array>
#include <game/comp/chunk_index.h>
#include <common/world/TextureManager.h>
#include <common/HashTuple.h>
#include <common/world/ChunkRender.h>

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
    std::array<entt::entity , WorldConfig::ChunkSize()> voxels;
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

bool has_trait(voxel_traits_t traits, VoxelTrait trait)
{
    return traits & trait;
}

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
};

std::unordered_map<ChunkIndex, entt::entity, hash_tuple> chunk_entity_map;
std::array<tex_t, (1u << 16)> texture_layers;
TextureManager texture_manager;

struct CoreEntity
{
    // Use entity_id not voxel_t
    static constexpr int Block_Start = 0;
    static constexpr entt::entity Block_Empty = static_cast<const entt::entity>(Block_Start + 1);
    static constexpr entt::entity Block_Dirt = static_cast<const entt::entity>(Block_Start + 2);
    static constexpr entt::entity Block_Grass = static_cast<const entt::entity>(Block_Start + 3);
    static constexpr entt::entity Block_Stone = static_cast<const entt::entity>(Block_Start + 4);
    // ...
};

struct NamedEntities
{
    inline static const std::string Block_Empty = "Core.Empty";
    inline static const std::string Block_Dirt = "Core.Dirt";
    inline static const std::string Block_Grass = "Core.Grass";
    inline static const std::string Block_Stone = "Core.Stone";
};

std::unordered_map<std::string, entt::entity> named_entities;

std::tuple<ChunkIndex, VoxelIndex> StandardizeVoxelIndex(const ChunkIndex& chunkIndex, UnboundVoxelIndex unboundIndex)
{
    if(WorldUtils::IsVoxelOutOfChunkBounds(unboundIndex))
    {
        // neighbour
        auto[index_x, index_y, index_z] = chunkIndex;
        int x = unboundIndex.x, y = unboundIndex.y, z = unboundIndex.z;
        if (x < 0)
        {
            index_x--;
            x = WorldConfig::kChunkSizeX - 1;
        }
        else if (x > WorldConfig::kChunkSizeX - 1)
        {
            index_x++;
            x = 0;
        }

        if (y < 0)
        {
            index_y--;
            y = WorldConfig::kChunkSizeY - 1;
        }
        else if (y > WorldConfig::kChunkSizeY - 1)
        {
            index_y++;
            y = 0;
        }

        if (z < 0)
        {
            index_z--;
            z = WorldConfig::kChunkSizeZ - 1;
        }
        else if (z > WorldConfig::kChunkSizeZ - 1)
        {
            index_z++;
            z = 0;
        }

        auto new_chunk_index = std::make_tuple(index_x, index_y, index_z);
        if (new_chunk_index == chunkIndex)
        {
            terminate();
//			UE_LOG(LogTemp, Warning, TEXT("Recursive chunk: %d,%d,%d"), index_x, index_y, index_z);
        }
        auto new_voxel_index = VoxelIndex(x, y, z);
        return std::make_tuple(new_chunk_index, new_voxel_index);
    }
    else
    {
        return std::make_tuple(chunkIndex, unboundIndex);
    }
}

void InitGameCore(entt::registry &registry)
{
    auto empty = registry.create(CoreEntity::Block_Empty);
    std::cout << "create entity " << static_cast<int>(empty) << "\n";
    auto dirt = registry.create(CoreEntity::Block_Dirt);
    auto grass = registry.create(CoreEntity::Block_Grass);
    auto stone = registry.create(CoreEntity::Block_Stone);

    named_entities[NamedEntities::Block_Empty] = empty;
    named_entities[NamedEntities::Block_Dirt] = dirt;
    named_entities[NamedEntities::Block_Grass] = grass;
    named_entities[NamedEntities::Block_Stone] = stone;

    registry.emplace<VoxelDataComponent>(empty, "Empty", static_cast<voxel_traits_t>(VoxelTrait::Empty));
    registry.emplace<VoxelDataComponent>(dirt, "Dirt", static_cast<voxel_traits_t>(VoxelTrait::Opaque));
    registry.emplace<VoxelDataComponent>(grass, "Grass", static_cast<voxel_traits_t>(VoxelTrait::Opaque));
    registry.emplace<VoxelDataComponent>(stone, "Stone", static_cast<voxel_traits_t>(VoxelTrait::Opaque));

    // Convert config to runtime
    registry.emplace<RuntimeVoxelTextureLayerComponent>(empty, VoxelTextureLayers{
            .front = 0, .back = 0,
            .left = 0, .right = 0,
            .top = 0, .bottom = 0,
    });
    auto dirt_layer = texture_manager.GetVoxelTextureLayer("dirt");
    registry.emplace<RuntimeVoxelTextureLayerComponent>(dirt, VoxelTextureLayers{
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
    registry.emplace<RuntimeVoxelTextureLayerComponent>(stone, VoxelTextureLayers{
            .front = stone_layer, .back = stone_layer,
            .left = stone_layer, .right = stone_layer,
            .top = stone_layer, .bottom = stone_layer,
    });
}

entt::entity get_chunk_entity(const ChunkIndex &chunkIndex)
{
    return chunk_entity_map[chunkIndex];
}

struct VoxelEntityGenerator
{
    auto operator()(const Position &chunkPosition)
    {
        std::array<entt::entity, WorldConfig::ChunkSize()> voxels;
        const int dirt_base = 60;
        for(int x = 0; x < WorldConfig::kChunkSizeX; ++x)
        {
            for(int z = 0; z < WorldConfig::kChunkSizeZ; ++z)
            {
                auto horizontal = NoiseTool::GenerateHeightWithCache(chunkPosition.x + x, chunkPosition.z + z);
                for(int y = 0; y < WorldConfig::kChunkSizeY; ++y)
                {
                    entt::entity voxel;
                    int global_height = chunkPosition.y + y;
                    if (global_height > horizontal)
                    {
                        voxel = named_entities[NamedEntities::Block_Empty];
                    }
                    else if (global_height == horizontal)
                    {
                        voxel = named_entities[NamedEntities::Block_Grass];
                    }
                    else if (global_height >= dirt_base)
                    {
                        voxel = named_entities[NamedEntities::Block_Dirt];
                    }
                    else
                    {
                        voxel = named_entities[NamedEntities::Block_Stone];
                    }
                    voxels[WorldUtils::voxel_index_to_data_index(VoxelIndex{x, y, z})] = voxel;
                }
            }
        }
        return voxels;
    }
};

struct ChunkInitSystem{
    static void Tick(std::function<std::array<entt::entity, WorldConfig::ChunkSize()> (const Position &)> generator,
                     entt::registry &registry) {
        auto view = registry.view<ChunkInitComponent>();
        for (auto entity : view) {
            auto &chunk_init = view.get<ChunkInitComponent>(entity);

            registry.emplace<ChunkIndexComponent>(entity, chunk_init.chunk_index);
            registry.emplace<ChunkVoxelEntitiesComponent>(entity, ChunkVoxelEntitiesComponent{.voxels = generator(
                    WorldUtils::ChunkIndexToPosition(chunk_init.chunk_index))});
            chunk_entity_map[chunk_init.chunk_index] = entity;
            registry.emplace<ChunkMeshInitComponent>(entity);
        }

        auto remove_view = registry.view<ChunkInitComponent>();
        for(auto entity : remove_view)
        {
            registry.remove<ChunkInitComponent>(entity);
        }
    }
};

// create chunk vertices and indices
struct ChunkMeshInitSystem {
    static void Tick(entt::registry &registry){
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

#endif //LEARNCRAFT_GAMEPLAY_H

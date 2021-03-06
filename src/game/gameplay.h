//
// Created by reili on 2020/9/30.
//

#ifndef LEARNCRAFT_GAMEPLAY_H
#define LEARNCRAFT_GAMEPLAY_H

#include <entt/entt.hpp>
#include <common/world/TextureManager.h>
#include <common/HashTuple.h>
#include <common/Camera.h>
#include <common/world/WorldUtils.h>

struct Game
{
    struct ChunkEntityChange
    {
        VoxelPosition pos;
        entt::entity old_entity;
        entt::entity new_entity;
    };

    void Init();
    void FixedTick(double fixedDeltaTime);
    void Tick(double deltaTime);
    void RenderScene(Camera &camera);
    void RenderUI();
    void HandleKeyboard(GLFWwindow *window);
    void SetVoxelEntity(const VoxelPosition &pos, entt::entity new_entity);
    void CollectChunksToUpdateMesh();

    entt::registry registry;
    TextureManager texture_manager;
    std::unordered_map<ChunkIndex, entt::entity, hash_tuple> chunk_entity_map;
    std::array<tex_t, (1u << 16)> texture_layers;
    std::unordered_map<std::string, entt::entity> named_entities;
    entt::entity player;

    glm::u8vec3 chunk_sight {2, 2, 2};

    Position player_last_pos;

    entt::entity GetVoxelEntity(const VoxelPosition &pos);

    std::optional<VoxelPosition> tryInteract(bool dig,
                                             const glm::vec3& position,
                                             const glm::vec3& rotation);


    std::optional<VoxelPosition> hit;
    std::vector<ChunkEntityChange> voxel_entities_changed;
    entt::entity voxel_to_place;
    std::vector<entt::entity> voxel_prototypes;
};

struct VoxelTexturePathComponent
{
    VoxelTextureNames textures;
};

//entt::entity get_chunk_entity(const ChunkIndex &chunkIndex)
//{
//    return chunk_entity_map[chunkIndex];
//}

#endif //LEARNCRAFT_GAMEPLAY_H

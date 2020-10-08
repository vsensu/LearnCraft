//
// Created by reili on 2020/10/7.
//

#include "gameplay.h"

#include <unordered_set>

#include <game/sys/ChunkRenderSystem.h>
#include <game/comp/VoxelDataComponent.h>
#include <game/comp/RuntimeVoxelTextureLayerComponent.h>
#include <game/sys/ChunkInitSystem.h>
#include <game/sys/ChunkMeshInitSystem.h>
#include <game/comp/RotationComponent.h>
#include <imgui.h>

#include "entities.h"
#include "Terrain.h"
#include "DrawDebug.h"

void Game::Init()
{
    // load textures
    texture_manager.RegisterTexture("Empty", "Textures/dirt.jpg");
    texture_manager.RegisterTexture("Dirt", "Textures/dirt.jpg");
    texture_manager.RegisterTexture("Grass.Top", "Textures/grass.jpg");
    texture_manager.RegisterTexture("Stone", "Textures/stone.jpg");
    GLuint texture = texture_manager.CreateTexture();
    ChunkRenderSystem::init(texture);

    // create core entities
    auto empty = registry.create(CoreEntity::Block_Empty);
    auto dirt = registry.create(CoreEntity::Block_Dirt);
    auto grass = registry.create(CoreEntity::Block_Grass);
    auto stone = registry.create(CoreEntity::Block_Stone);

    named_entities[NamedEntities::Block_Empty] = empty;
    named_entities[NamedEntities::Block_Dirt] = dirt;
    named_entities[NamedEntities::Block_Grass] = grass;
    named_entities[NamedEntities::Block_Stone] = stone;

    // set voxel traits
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
    auto dirt_layer = texture_manager.GetVoxelTextureLayer("Dirt");
    registry.emplace<RuntimeVoxelTextureLayerComponent>(dirt, VoxelTextureLayers{
            .front = dirt_layer, .back = dirt_layer,
            .left = dirt_layer, .right = dirt_layer,
            .top = dirt_layer, .bottom = dirt_layer,
    });
    registry.emplace<RuntimeVoxelTextureLayerComponent>(grass, VoxelTextureLayers{
            .front = dirt_layer, .back = dirt_layer,
            .left = dirt_layer, .right = dirt_layer,
            .top = texture_manager.GetVoxelTextureLayer("Grass.Top"),
            .bottom = dirt_layer,
    });
    auto stone_layer = texture_manager.GetVoxelTextureLayer("Stone");
    registry.emplace<RuntimeVoxelTextureLayerComponent>(stone, VoxelTextureLayers{
            .front = stone_layer, .back = stone_layer,
            .left = stone_layer, .right = stone_layer,
            .top = stone_layer, .bottom = stone_layer,
    });

    // generate terrain
    NoiseTool::GenerateHeightCache(-50, -50, 50, 50);
    int center_x = 0, center_z = 0;
    int initSize = 5;
    int chunk_vertical = 16;

    for (int x = center_x - initSize; x < center_x + initSize; ++x) {
        for (int z = center_z - initSize; z < center_z + initSize; ++z) {
            for (int y = 0; y < chunk_vertical; ++y) {
                ChunkIndex chunk_index{x, y, z};
                auto entity = registry.create();
                registry.emplace<ChunkInitComponent>(entity, ChunkInitComponent{.chunk_index = chunk_index});
            }
        }
    }

    // Tick at begin
    Tick(0.f);

    // get player spawn location
    auto height = NoiseTool::GenerateHeightWithCache(0, 0);

    // create player
    player = registry.create();
    registry.emplace<PositionComponent>(player, PositionComponent{.val = Position{0, height, 0}});
    registry.emplace<RotationComponent>(player, RotationComponent{.val = {0, 0, 0}});
    player_last_pos = {0, 0, 0};

    DrawDebug::Init();
}

void Game::FixedTick(double fixedDeltaTime)
{

}

void Game::Tick(double deltaTime)
{
    VoxelEntityGenerator generator;
    ChunkInitSystem::Tick(chunk_entity_map, generator, registry);
    ChunkMeshInitSystem::Tick(registry, chunk_entity_map);
}

void Game::RenderScene(Camera &camera)
{
    auto &pos = registry.get<PositionComponent>(player);
    auto &rot = registry.get<RotationComponent>(player);
    pos.val = camera.GetPos();
    rot.val = camera.GetForward();

    auto last_chunk_index = WorldUtils::GetChunkIndexViaLocation(player_last_pos);
    auto curr_chunk_index = WorldUtils::GetChunkIndexViaLocation(pos.val);
    auto [last_chunk_index_x, last_chunk_index_y, last_chunk_index_z] = last_chunk_index;
    auto [curr_chunk_index_x, curr_chunk_index_y, curr_chunk_index_z] = curr_chunk_index;
    if(last_chunk_index_x != curr_chunk_index_x || last_chunk_index_y != curr_chunk_index_y || last_chunk_index_z != curr_chunk_index_z)
    {
        int dx = curr_chunk_index_x - last_chunk_index_x;
        int dy = curr_chunk_index_y - last_chunk_index_y;
        int dz = curr_chunk_index_z - last_chunk_index_z;

        int nx = dx > 0 ? 1 : -1;
        int ny = dy > 0 ? 1 : -1;
        int nz = dz > 0 ? 1 : -1;

        int ox = last_chunk_index_x + nx * static_cast<int>(chunk_sight.x);
        int oy = last_chunk_index_y + ny * static_cast<int>(chunk_sight.y);
        int oz = last_chunk_index_z + nz * static_cast<int>(chunk_sight.z);
        int tx = ox + dx;
        int ty = oy + dy;
        int tz = oz + dz;

        std::unordered_set<ChunkIndex, hash_tuple> chunks_to_create;
        for(int x = ox + nx; x != tx+nx; x += nx)
        {
            for(int y = -static_cast<int>(chunk_sight.y); y <= static_cast<int>(chunk_sight.y); ++y)
            {
               for(int z = -static_cast<int>(chunk_sight.z); z <= static_cast<int>(chunk_sight.z); ++z)
               {
                   ChunkIndex chunkIndex{x, curr_chunk_index_y+y, curr_chunk_index_z+z};
                   chunks_to_create.emplace(chunkIndex);
               }
            }
        }
        for(int z = oz + nz; z != tz+nz; z += nz)
        {
            for(int y = -static_cast<int>(chunk_sight.y); y <= static_cast<int>(chunk_sight.y); ++y)
            {
                for(int x = -static_cast<int>(chunk_sight.x); x <= static_cast<int>(chunk_sight.x); ++x)
                {
                    ChunkIndex chunkIndex{curr_chunk_index_x+x, curr_chunk_index_y+y, z};
                    chunks_to_create.emplace(chunkIndex);
                }
            }
        }
        for(int y = oy + ny; y != ty+ny; y += ny)
        {
            for(int x = -static_cast<int>(chunk_sight.x); x <= static_cast<int>(chunk_sight.x); ++x)
            {
                for(int z = -static_cast<int>(chunk_sight.z); z <= static_cast<int>(chunk_sight.z); ++z)
                {
                    ChunkIndex chunkIndex{curr_chunk_index_x+x, y, curr_chunk_index_z+z};
                    chunks_to_create.emplace(chunkIndex);
                }
            }
        }
        for(const auto &chunkIndex : chunks_to_create){
            if (chunk_entity_map.find(chunkIndex) == chunk_entity_map.end()) {
                auto entity = registry.create();
                registry.emplace<ChunkInitComponent>(entity, ChunkInitComponent{.chunk_index=chunkIndex});
            }
        }
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ChunkRenderSystem::Tick(camera, registry, 80.f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    DrawDebug::SetProjView(camera.Perspective(800.f/600) * camera.View());
    DrawDebug::Render();

    player_last_pos = pos.val;
}

void Game::RenderUI()
{
    ImGui::Begin("Game");
    ImGui::Text("draw vertex:%lu", ChunkRenderSystem::vertex_draw_count);
    auto &pos = registry.get<PositionComponent>(player);
    auto &rot = registry.get<RotationComponent>(player);
    hit = tryInteract(true, pos.val, rot.val);
    if(hit.has_value())
    {
        auto hit_entity = GetVoxelEntity(hit.value());
        auto &hit_data = registry.get<VoxelDataComponent>(hit_entity);
        ImGui::Text("Hit: %s", hit_data.name.c_str());

        DrawDebug::DrawDebugBox(hit.value());
    }
    ImGui::End();
}

entt::entity Game::GetVoxelEntity(const VoxelPosition &pos) {
    auto[chunkIndex, voxelIndex] = WorldUtils::GlobalLocationToVoxel(pos);
    auto chunk_entity = chunk_entity_map[chunkIndex];
    auto & voxel_entities = registry.get<ChunkVoxelEntitiesComponent>(chunk_entity);
    auto voxel_entity = voxel_entities.voxels[WorldUtils::voxel_index_to_data_index(voxelIndex)];
    return voxel_entity;
}

std::optional<VoxelPosition> Game::tryInteract(bool dig,
                                               const glm::vec3 &position,
                                               const glm::vec3 &forward) {
//    auto voxelPositions = WorldUtils::getIntersectedVoxels(position, WorldUtils::forwardsVector(rotation), 8);
    auto voxelPositions = WorldUtils::getIntersectedVoxels(position, forward, 8);
    if (voxelPositions.empty()) {
        return {};
    }
    VoxelPosition &previous = voxelPositions.at(0);

    for (auto &voxelPosition : voxelPositions) {
        auto voxel_entity = GetVoxelEntity(voxelPosition);
        auto &voxel_data = registry.get<VoxelDataComponent>(voxel_entity);
        if (has_trait_opaque(voxel_data.traits)) {
            return dig ? voxelPosition : previous;
        }
        previous = voxelPosition;
    }
    return {};
}

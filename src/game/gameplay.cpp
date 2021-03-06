//
// Created by reili on 2020/10/7.
//

#include "gameplay.h"

#include <unordered_set>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

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
#include "ExportToLua.h"

#include <sol/sol.hpp>

bool demo_exists(const fs::path& p, fs::file_status s = fs::file_status{})
{
    return fs::status_known(s) ? fs::exists(s) : fs::exists(p);
}

std::vector<std::string> GetFirstLayerDirs(const std::string &root, bool absolute)
{
    std::vector<std::string> dirs;
    if (demo_exists(root))
    {
        for (auto &p : fs::directory_iterator(root))
        {
            if (fs::is_directory(p.path()))
            {
                dirs.emplace_back(absolute ? fs::absolute(p.path()).generic_string() : p.path().generic_string());
                //            auto mod_init_path = p.path() / "mod.lua";
                //            if(demo_exists(mod_init_path))
                //            {
                //                mods.emplace_back(mod_init_path.generic_string());
                //                std::cout << "find mod " << mod_init_path.generic_string() << "\n";
                //            }
            }
        }
    }

    return dirs;
}

bool PathExists(const std::string &path)
{
    return demo_exists(path);
}

void Game::Init()
{
    sol::state lua;
    lua.open_libraries();

    sol::usertype<TextureManager> texture_manager_type = lua.new_usertype<TextureManager>("TextureManager");
    texture_manager_type["RegisterTexture"] = &TextureManager::RegisterTexture;
    texture_manager_type["GetVoxelTextureLayer"] = &TextureManager::GetVoxelTextureLayer;

    sol::usertype<entt::registry> registry_type = lua.new_usertype<entt::registry>("Registry");
    entt::entity (entt::registry::*create_entity)() = &entt::registry::create;
    entt::entity (entt::registry::*create_entity_hint)(const entt::entity) = &entt::registry::create;
    registry_type["create"] = create_entity;
    registry_type["create_with_hint"] = create_entity_hint;

    lua["texture_manager"] = &texture_manager;
    lua["registry"] = &registry;

    lua["EmplaceVoxelDataComponent"] = &ExportToLua::EmplaceVoxelDataComponent;
    lua["EmplaceRuntimeVoxelTextureLayerComponent"] = &ExportToLua::EmplaceRuntimeVoxelTextureLayerComponent;

    lua["AddVoxelPrototype"] = &ExportToLua::AddVoxelPrototype;
    lua["CreateTexture"] = &ExportToLua::CreateTexture;
    lua["voxel_prototypes"] = &voxel_prototypes;

    lua["CustomBlockStart"] = CoreEntity::Block_Custom;

    lua["Trait_Empty"] = VoxelTrait::Empty;
    lua["Trait_Opaque"] = VoxelTrait::Opaque;

    lua["GetFirstLayerDirs"] = &GetFirstLayerDirs;
    lua["PathExists"] = &PathExists;

    lua.script_file("init.lua");

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

    auto &pos = registry.get<PositionComponent>(player);
    auto &rot = registry.get<RotationComponent>(player);
    hit = tryInteract(true, pos.val, rot.val);
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

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ChunkRenderSystem::Tick(camera, registry, 80.f, texture_manager.GetTextureNum());

//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    DrawDebug::SetProjView(camera.Perspective(800.f/600) * camera.View());
    DrawDebug::Render();

    CollectChunksToUpdateMesh();

    player_last_pos = pos.val;
}

void Game::RenderUI()
{
//    ImGui::ShowDemoWindow();
    ImGui::Begin("Game");
    ImGui::Text("draw vertex:%lu", ChunkRenderSystem::vertex_draw_count);

    if(hit.has_value())
    {
        auto hit_entity = GetVoxelEntity(hit.value());
        auto &hit_data = registry.get<VoxelDataComponent>(hit_entity);
        ImGui::Text("Hit: %s", hit_data.name.c_str());
        DrawDebug::DrawDebugBox(hit.value());
    }

    static int e = static_cast<int>(CoreEntity::Block_Dirt);
    for (auto voxel_prototype_entity : voxel_prototypes) {
        const auto &voxel_data = registry.get<VoxelDataComponent>(voxel_prototype_entity);
        const auto &texture_layer = registry.get<RuntimeVoxelTextureLayerComponent>(voxel_prototype_entity);
        auto [uv_min, uv_max] = texture_manager.GetUV(texture_layer.layers.front);
        ImGui::Image((void *) (intptr_t) texture_manager.texture,
                     ImVec2(texture_manager.unit_width, texture_manager.unit_height), ImVec2(uv_min.x, uv_min.y),
                     ImVec2(uv_max.x, uv_max.y));
        ImGui::SameLine();
        ImGui::RadioButton(voxel_data.name.c_str(), &e, static_cast<int>(voxel_prototype_entity));
    }
    voxel_to_place = static_cast<entt::entity>(e);
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

void Game::SetVoxelEntity(const VoxelPosition &pos, entt::entity new_entity) {
    auto[chunkIndex, voxelIndex] = WorldUtils::GlobalLocationToVoxel(pos);
    auto chunk_entity = chunk_entity_map[chunkIndex];
    auto &voxel_entities = registry.get<ChunkVoxelEntitiesComponent>(chunk_entity);
    auto &voxel_entity = voxel_entities.voxels[WorldUtils::voxel_index_to_data_index(voxelIndex)];
    entt::entity old_entity = voxel_entity;
    voxel_entity = new_entity;
    voxel_entities_changed.push_back(
            ChunkEntityChange{.pos = pos, .old_entity = old_entity, .new_entity = new_entity});
}

void Game::HandleKeyboard(GLFWwindow *window)
{
    static bool mouse_left_pressed = false;
    static bool mouse_right_pressed = false;
    if(!mouse_left_pressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        mouse_left_pressed = true;
        if(hit.has_value())
        {
            SetVoxelEntity(hit.value(), CoreEntity::Block_Empty);
        }
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
    {
        mouse_left_pressed = false;
    }

    if(!mouse_right_pressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    {
        mouse_right_pressed = true;
        auto &pos = registry.get<PositionComponent>(player);
        auto &rot = registry.get<RotationComponent>(player);
        hit = tryInteract(false, pos.val, rot.val);
        if(hit.has_value())
        {
            SetVoxelEntity(hit.value(), voxel_to_place);
        }
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
    {
        mouse_right_pressed = false;
    }
}

void Game::CollectChunksToUpdateMesh()
{
    std::set<ChunkIndex> chunks_mesh_to_update;
    for(const auto &voxel_changed : voxel_entities_changed)
    {
        auto[chunkIndex, voxelIndex] = WorldUtils::GlobalLocationToVoxel(voxel_changed.pos);
        chunks_mesh_to_update.emplace(chunkIndex);

        auto [chunk_index_x, chunk_index_y, chunk_index_z] = chunkIndex;

        if(voxelIndex.x == 0)
            chunks_mesh_to_update.emplace(ChunkIndex{chunk_index_x-1, chunk_index_y, chunk_index_z});
        if(voxelIndex.x+1 == WorldConfig::kChunkSizeX)
            chunks_mesh_to_update.emplace(ChunkIndex{chunk_index_x+1, chunk_index_y, chunk_index_z});
        if(voxelIndex.y == 0)
            chunks_mesh_to_update.emplace(ChunkIndex{chunk_index_x, chunk_index_y-1, chunk_index_z});
        if(voxelIndex.y+1 == WorldConfig::kChunkSizeY)
            chunks_mesh_to_update.emplace(ChunkIndex{chunk_index_x, chunk_index_y+1, chunk_index_z});
        if(voxelIndex.z == 0)
            chunks_mesh_to_update.emplace(ChunkIndex{chunk_index_x, chunk_index_y, chunk_index_z-1});
        if(voxelIndex.z+1 == WorldConfig::kChunkSizeZ)
            chunks_mesh_to_update.emplace(ChunkIndex{chunk_index_x, chunk_index_y, chunk_index_z+1});
    }
    for(const auto &chunk_mesh_to_update : chunks_mesh_to_update)
    {
        auto chunk_entity = chunk_entity_map[chunk_mesh_to_update];
        registry.remove<ChunkRenderComponent>(chunk_entity);
        registry.emplace<ChunkMeshInitComponent>(chunk_entity);
    }
}

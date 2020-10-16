//
// Created by reili on 2020/10/16.
//

#ifndef LEARNCRAFT_EXPORTTOLUA_H
#define LEARNCRAFT_EXPORTTOLUA_H

#include <sol/sol.hpp>
#include <entt/entt.hpp>
#include <game/comp/VoxelDataComponent.h>

namespace ExportToLua
{
    void EmplaceVoxelDataComponent(entt::registry &registry, entt::entity entity, const std::string &name, voxel_traits_t traits)
    {
        registry.emplace<VoxelDataComponent>(entity, name, traits);
    }

    void CreateTexture(TextureManager &texture_manager)
    {
        GLuint texture = texture_manager.CreateTexture();
        ChunkRenderSystem::init(texture);
    }

    void AddVoxelPrototype(std::vector<entt::entity> &voxel_prototypes, entt::entity entity)
    {
        voxel_prototypes.emplace_back(entity);
    }

    void EmplaceRuntimeVoxelTextureLayerComponent(entt::registry &registry, entt::entity entity, tex_t front, tex_t back, tex_t left, tex_t right, tex_t top, tex_t bottom)
    {
        registry.emplace<RuntimeVoxelTextureLayerComponent>(entity, VoxelTextureLayers{.front = front, .back = back, .left = left, .right = right, .top = top, .bottom = bottom});
    }
}

#endif //LEARNCRAFT_EXPORTTOLUA_H

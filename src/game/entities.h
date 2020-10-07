//
// Created by reili on 2020/10/7.
//

#ifndef LEARNCRAFT_ENTITIES_H
#define LEARNCRAFT_ENTITIES_H

#include <string>
#include <entt/entt.hpp>

/*
 * define ENTT_ID_TYPE std::uint32_t
 * ===================================
 * Config(Type) is confirmed instance.
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

#endif //LEARNCRAFT_ENTITIES_H

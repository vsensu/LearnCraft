--
-- User: vsensu
-- Date: 2020/10/19
--

LuaAPI = {}
LuaAPI.RegisterTexture = function(name, path)
    texture_manager:RegisterTexture(name, path);
end
LuaAPI.CreateTexture = function()
    CreateTexture(texture_manager);
end
LuaAPI.CreateEntityWithHint = function(hint)
    return registry:create_with_hint(hint);
end
LuaAPI.AddVoxelPrototype = function(prototype)
    AddVoxelPrototype(voxel_prototypes, prototype);
end
LuaAPI.EmplaceVoxelDataComponent = function(entity, name, traits)
    EmplaceVoxelDataComponent(registry, entity, name, traits);
end
LuaAPI.GetVoxelTextureLayer = function(name)
    return texture_manager:GetVoxelTextureLayer(name);
end
LuaAPI.EmplaceRuntimeVoxelTextureLayerComponent = function(entity, front, back, left, right, top, bottom)
    EmplaceRuntimeVoxelTextureLayerComponent(registry, entity, front, back, left, right, top, bottom);
end

LuaAPI.Trait = { Empty = 1, }

LuaAPI.CoreEntity = {
    DebugBorder = 0,
    Empty = 1,
    Dirt = 2,
    Grass = 3,
    Stone = 4,
}

LuaAPI.GetFirstLayerDirs = GetFirstLayerDirs;
LuaAPI.PathExists = PathExists;

return LuaAPI
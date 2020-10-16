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

print("load textures from lua...")
LuaAPI.RegisterTexture("_DEBUG_BORDER", "Textures/debug_border.jpg");
LuaAPI.RegisterTexture("Empty", "Textures/dirt.jpg");
LuaAPI.RegisterTexture("Dirt", "Textures/dirt.jpg");
LuaAPI.RegisterTexture("Grass.Top", "Textures/grass.jpg");
LuaAPI.RegisterTexture("Grass.Side", "Textures/grass_side.jpg");
LuaAPI.RegisterTexture("Stone", "Textures/stone.jpg");
LuaAPI.RegisterTexture("Log.Top", "Textures/log_top.jpg");
LuaAPI.RegisterTexture("Log.Side", "Textures/log_side.jpg");
LuaAPI.RegisterTexture("Gold", "Textures/gold_block.jpg");
LuaAPI.CreateTexture();

gold = LuaAPI.CreateEntityWithHint(CustomBlockStart);

LuaAPI.AddVoxelPrototype(gold);

LuaAPI.EmplaceVoxelDataComponent(gold, "Gold", Trait_Opaque);

gold_layer = LuaAPI.GetVoxelTextureLayer("Gold");

LuaAPI.EmplaceRuntimeVoxelTextureLayerComponent(gold, gold_layer, gold_layer, gold_layer, gold_layer, gold_layer, gold_layer);

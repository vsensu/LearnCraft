--
-- User: vsensu
-- Date: 2020/10/19
--

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

debug_border = LuaAPI.CreateEntityWithHint(LuaAPI.CoreEntity.DebugBorder);
empty = LuaAPI.CreateEntityWithHint(LuaAPI.CoreEntity.Empty);
dirt = LuaAPI.CreateEntityWithHint(LuaAPI.CoreEntity.Dirt);
grass = LuaAPI.CreateEntityWithHint(LuaAPI.CoreEntity.Grass);
stone = LuaAPI.CreateEntityWithHint(LuaAPI.CoreEntity.Stone);
gold = LuaAPI.CreateEntityWithHint(CustomBlockStart);

LuaAPI.AddVoxelPrototype(debug_border);
LuaAPI.AddVoxelPrototype(empty);
LuaAPI.AddVoxelPrototype(dirt);
LuaAPI.AddVoxelPrototype(grass);
LuaAPI.AddVoxelPrototype(stone);
LuaAPI.AddVoxelPrototype(gold);

LuaAPI.EmplaceVoxelDataComponent(debug_border, "DEBUG_BORDER", Trait_Opaque);
LuaAPI.EmplaceVoxelDataComponent(empty, "Empty", LuaAPI.Trait.Empty);
LuaAPI.EmplaceVoxelDataComponent(dirt, "Dirt", Trait_Opaque);
LuaAPI.EmplaceVoxelDataComponent(grass, "Grass", Trait_Opaque);
LuaAPI.EmplaceVoxelDataComponent(stone, "Stone", Trait_Opaque);
LuaAPI.EmplaceVoxelDataComponent(gold, "Gold", Trait_Opaque);

debug_border_layer = LuaAPI.GetVoxelTextureLayer("_DEBUG_BORDER");
empty_layer = LuaAPI.GetVoxelTextureLayer("Empty");
dirt_layer = LuaAPI.GetVoxelTextureLayer("Dirt");
grass_side_layer = LuaAPI.GetVoxelTextureLayer("Grass.Side");
grass_top_layer = LuaAPI.GetVoxelTextureLayer("Grass.Top");
stone_layer = LuaAPI.GetVoxelTextureLayer("Stone");
gold_layer = LuaAPI.GetVoxelTextureLayer("Gold");

LuaAPI.EmplaceRuntimeVoxelTextureLayerComponent(debug_border, debug_border_layer, debug_border_layer, debug_border_layer, debug_border_layer, debug_border_layer, debug_border_layer);
LuaAPI.EmplaceRuntimeVoxelTextureLayerComponent(empty, empty_layer, empty_layer, empty_layer, empty_layer, empty_layer, empty_layer);
LuaAPI.EmplaceRuntimeVoxelTextureLayerComponent(dirt, dirt_layer, dirt_layer, dirt_layer, dirt_layer, dirt_layer, dirt_layer);
LuaAPI.EmplaceRuntimeVoxelTextureLayerComponent(grass, grass_side_layer, grass_side_layer, grass_side_layer, grass_side_layer, grass_top_layer, dirt_layer);
LuaAPI.EmplaceRuntimeVoxelTextureLayerComponent(stone, stone_layer, stone_layer, stone_layer, stone_layer, stone_layer, stone_layer);
LuaAPI.EmplaceRuntimeVoxelTextureLayerComponent(gold, gold_layer, gold_layer, gold_layer, gold_layer, gold_layer, gold_layer);

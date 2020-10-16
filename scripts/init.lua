print("load textures from lua...")
texture_manager:RegisterTexture("_DEBUG_BORDER", "Textures/debug_border.jpg");
texture_manager:RegisterTexture("Empty", "Textures/dirt.jpg");
texture_manager:RegisterTexture("Dirt", "Textures/dirt.jpg");
texture_manager:RegisterTexture("Grass.Top", "Textures/grass.jpg");
texture_manager:RegisterTexture("Grass.Side", "Textures/grass_side.jpg");
texture_manager:RegisterTexture("Stone", "Textures/stone.jpg");
texture_manager:RegisterTexture("Log.Top", "Textures/log_top.jpg");
texture_manager:RegisterTexture("Log.Side", "Textures/log_side.jpg");
texture_manager:RegisterTexture("Gold", "Textures/gold_block.jpg");
CreateTexture(texture_manager);

gold = registry:create_with_hint(CustomBlockStart);

AddVoxelPrototype(voxel_prototypes, gold);

EmplaceVoxelDataComponent(registry, gold, "Gold", Trait_Opaque);

gold_layer = texture_manager:GetVoxelTextureLayer("Gold");

EmplaceRuntimeVoxelTextureLayerComponent(registry, gold, gold_layer, gold_layer, gold_layer, gold_layer, gold_layer, gold_layer);

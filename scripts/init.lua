print("load textures from lua")
texture_manager:RegisterTexture("_DEBUG_BORDER", "Textures/debug_border.jpg");
texture_manager:RegisterTexture("Empty", "Textures/dirt.jpg");
texture_manager:RegisterTexture("Dirt", "Textures/dirt.jpg");
texture_manager:RegisterTexture("Grass.Top", "Textures/grass.jpg");
texture_manager:RegisterTexture("Grass.Side", "Textures/grass_side.jpg");
texture_manager:RegisterTexture("Stone", "Textures/stone.jpg");
a = registry:create_with_hint(8);
b = "lua_table";
add_lua_comp(registry, a, b);
print("lua create entity and add component");
--
-- User: vsensu
-- Date: 2020/10/19
--

print("read mods...")

Mods = {}

dirs = LuaAPI.GetFirstLayerDirs("Mods", false)
for i, v in ipairs(dirs) do
    path = v .. "/mod.lua"
    if LuaAPI.PathExists(path) then
        mod = dofile(path)
        print(mod.name, mod.author)
    end
end

return Mods;
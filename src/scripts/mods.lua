--
-- User: vsensu
-- Date: 2020/10/19
--

LuaAPI = require("api")

print("read mods...")

Mods = {}

Mods.List = {}
Mods.InitList = {}

Mods.Load = function(path)
    dirs = LuaAPI.GetFirstLayerDirs(path, false)
    for i, v in ipairs(dirs) do
        path = v .. "/mod.lua"
        if LuaAPI.PathExists(path) then
            mod = dofile(path)
            mod.basepath = v
            key = mod.name .. "@" .. mod.author
            Mods.List[key] = mod
            if mod.init ~= nil then
                Mods.InitList[key .. ".init"] = mod.basepath .. "/" .. mod.init
            end
        end
    end
end

Mods.Init = function()
    for i, v in pairs(Mods.InitList) do
        print(i, v)
        dofile(v)
    end
end

LuaAPI.ModDoFile = function(mod, relative_path)
    dofile(mod.basepath .. "/" .. relative_path)
end

LuaAPI.ModRequire = function(mod, relative_path)
    return require(mod.basepath .. "/" .. relative_path)
end

return Mods;

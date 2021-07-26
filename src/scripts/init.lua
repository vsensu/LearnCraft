LuaAPI = require("api")
Mods = require("mods")

Mods.Load("game")
Mods.Load("Mods")

for i, v in pairs(Mods.List) do
    print(i, v.version)
end

Mods.Init()


--
-- User: vsensu
-- Date: 2020/10/19
--

LuaAPI = require("api")
Mods = require("mods")
Mod = Mods.List["core@vsensu"]

LuaAPI.ModDoFile(Mod, "voxels.lua")

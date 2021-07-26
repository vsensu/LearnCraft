add_requires("entt 3.7.1", {alias = "entt"})
add_requires("glfw 3.3.2", {alias = "glfw"})
add_requires("glm 0.9.9+8", {alias = "glm"})
add_requires("conan::imgui/1.78", {alias = "imgui"})
add_requires("sol2 3.2.1", {alias = "sol2"})

target("LearnCraft")
    set_kind("binary")
    set_languages("c++20")
    add_includedirs("deps","deps/glad/include", "src", "src/platforms")
    add_files("deps/**/*.c", "deps/**/*.cpp", "src/**/*.cpp", "src/main.cpp")
    add_packages("entt", "glfw", "glm", "imgui", "sol2")
    if is_plat("windows") then 
        add_ldflags("-subsystem:windows", "-entry:mainCRTStartup", {force = true}) 
        -- glfw3.lib need
        add_links("user32", "gdi32", "shell32")
    end
    after_build(function (target)
        -- copy lua
        os.cp("src/scripts/api.lua", path.join(target:targetdir(), "api.lua"))
        os.cp("src/scripts/init.lua", path.join(target:targetdir(), "init.lua"))
        os.cp("src/scripts/mods.lua", path.join(target:targetdir(), "mods.lua"))

        -- copy core mods
        os.tryrm(path.join(target:targetdir(), "game/"))
        os.cp("src/scripts/game/", path.join(target:targetdir(), "game"))

        -- copy textures
        os.tryrm(path.join(target:targetdir(), "Textures/"))
        os.cp("res/textures/", path.join(target:targetdir(), "Textures"))
    end)

add_requires("conan::entt/3.5.2", {alias = "entt"})
add_requires("conan::glfw/3.3.2", {alias = "glfw"})
add_requires("conan::glm/0.9.9.8", {alias = "glm"})
add_requires("conan::imgui/1.78", {alias = "imgui"})
add_requires("conan::enet/1.3.16", {alias = "enet"})
add_requires("conan::sol2/3.2.1", {alias = "sol2"})

target("LearnCraft")
    set_kind("binary")
    set_languages("c++20")
    add_includedirs("deps/glad/include")
    add_includedirs("deps")
    add_includedirs("src/platforms")
    add_includedirs("src")
    add_files("deps/**/*.c")
    add_files("deps/**/*.cpp")
    add_files("src/**/*.cpp")
    add_files("src/main.cpp")
    add_packages("entt", "glfw", "glm", "imgui", "enet", "sol2")
    if is_plat("windows") then 
        add_ldflags("-subsystem:windows", "-entry:mainCRTStartup", {force = true}) 
        -- glfw3.lib need
        add_links("user32", "gdi32", "shell32")
    end

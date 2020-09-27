cmake_minimum_required(VERSION 2.8.12)
project(LearnCraft)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(
        deps/glad/include
        deps
        src/platforms
        src
)

add_executable(LearnCraft src/main.cpp deps/glad/src/glad.c src/platforms/imgui_impl_glfw.cpp src/platforms/imgui_impl_opengl3.cpp
        deps/stb_image/stb_image.cpp deps/stb_image/stb_image.h
        src/platforms/imgui_impl_glfw.h src/platforms/imgui_impl_opengl3.h
        src/common/gl_errors.cpp src/common/gl_errors.h
        src/common/world/WorldUtils.cpp src/common/world/WorldUtils.h
        src/common/world/ChunkRender.h
        src/common/world/ChunkGenerator.h
        src/common/world/TextureManager.h
        src/common/world/World.cpp src/common/world/Chunk.cpp src/common/NoiseTool.cpp src/common/Perlin_Noise/ppm.cpp
        src/common/Perlin_Noise/PerlinNoise.cpp src/common/world/VoxelManager.h
        src/common/Application.cpp src/game/GameApplication.cpp)
target_link_libraries(LearnCraft ${CONAN_LIBS})


enable_testing()
add_subdirectory(tests)

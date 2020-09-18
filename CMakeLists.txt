cmake_minimum_required(VERSION 2.8.12)
project(LearnCraft)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(
        deps/glad/include
        src/platforms
)

add_executable(LearnCraft src/main.cpp deps/glad/src/glad.c src/platforms/imgui_impl_glfw.cpp src/platforms/imgui_impl_opengl3.cpp
        src/platforms/imgui_impl_glfw.h src/platforms/imgui_impl_opengl3.h)
target_link_libraries(LearnCraft ${CONAN_LIBS})

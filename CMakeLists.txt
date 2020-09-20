cmake_minimum_required(VERSION 2.8.12)
project(LearnCraft)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(
        deps/glad/include
        src/platforms
        src
)

add_executable(LearnCraft src/main.cpp deps/glad/src/glad.c src/platforms/imgui_impl_glfw.cpp src/platforms/imgui_impl_opengl3.cpp
        src/platforms/imgui_impl_glfw.h src/platforms/imgui_impl_opengl3.h
        src/common/Application.cpp src/game/GameApplication.cpp)
target_link_libraries(LearnCraft ${CONAN_LIBS})


enable_testing()
add_subdirectory(tests)

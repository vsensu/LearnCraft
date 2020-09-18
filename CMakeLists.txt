cmake_minimum_required(VERSION 3.16)
project(LearnCraft)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(deps/glfw-3.3.2)

set(SFML_OS_WINDOWS ON CACHE BOOL "enable X functionality")
add_subdirectory(deps/SFML-2.5.1)

# .h
include_directories(
        src
        deps/glad/include
        deps/glad/include/KHR
        deps/glfw-3.3.2/include
        deps/SFML-2.5.1/include
        deps/imgui-1.78
        deps/imgui-1.78/examples
)

# lib folder
link_directories(
        ${CMAKE_CURRENT_BINARY_DIR}/deps/glfw-3.3.2/src
        ${CMAKE_CURRENT_BINARY_DIR}/deps/SFML-2.5.1/lib
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(
            deps/SFML-2.5.1/build/lib/Debug
    )
else()
    link_directories(
            deps/SFML-2.5.1/build/lib/Release
    )
endif()

# 将当前目录的所有源文件添加进变量
#aux_source_directory(. DIRS_SRCS)

add_executable(LearnCraft WIN32 src/main.cpp deps/imgui-1.78/imgui_demo.cpp deps/imgui-1.78/imgui_draw.cpp deps/imgui-1.78/imgui_internal.h
        deps/imgui-1.78/imgui_widgets.cpp
        deps/imgui-1.78/imgui.cpp deps/imgui-1.78/examples/imgui_impl_glfw.cpp deps/imgui-1.78/examples/imgui_impl_opengl3.cpp
        ${PROJECT_SOURCE_DIR}/deps/glad/src/glad.c)

# lib
target_link_libraries(LearnCraft
        glfw3
        )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(LearnCraft
            sfml-graphics-d
            sfml-system-d
            sfml-audio-d
            sfml-window-d
            sfml-network-d
            )
else ()
    target_link_libraries(LearnCraft
            sfml-graphics
            sfml-system
            sfml-audio
            sfml-window
            sfml-network
            )
endif ()

set(GUI_ONLY_BINARIES LearnCraft)
if (MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${GUI_ONLY_BINARIES} PROPERTIES
            LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

# post build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET LearnCraft
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML-2.5.1/build/lib/Debug/sfml-graphics-d-2.dll ${CMAKE_CURRENT_BINARY_DIR}/sfml-graphics-d-2.dll
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML-2.5.1/build/lib/Debug/sfml-system-d-2.dll ${CMAKE_CURRENT_BINARY_DIR}/sfml-system-d-2.dll
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML-2.5.1/build/lib/Debug/sfml-audio-d-2.dll ${CMAKE_CURRENT_BINARY_DIR}/sfml-audio-d-2.dll
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML-2.5.1/build/lib/Debug/sfml-window-d-2.dll ${CMAKE_CURRENT_BINARY_DIR}/sfml-window-d-2.dll
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML-2.5.1/build/lib/Debug/sfml-network-d-2.dll ${CMAKE_CURRENT_BINARY_DIR}/sfml-network-d-2.dll
            )
else ()
    add_custom_command(TARGET LearnCraft
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML-2.5.1/build/lib/Debug/sfml-graphics-2.dll ${CMAKE_CURRENT_BINARY_DIR}/sfml-graphics-2.dll
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML-2.5.1/build/lib/Debug/sfml-system-2.dll ${CMAKE_CURRENT_BINARY_DIR}/sfml-system-2.dll
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML-2.5.1/build/lib/Debug/sfml-audio-2.dll ${CMAKE_CURRENT_BINARY_DIR}/sfml-audio-2.dll
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML-2.5.1/build/lib/Debug/sfml-window-2.dll ${CMAKE_CURRENT_BINARY_DIR}/sfml-window-2.dll
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/SFML-2.5.1/build/lib/Debug/sfml-network-2.dll ${CMAKE_CURRENT_BINARY_DIR}/sfml-network-2.dll
            )
endif ()



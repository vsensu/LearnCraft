conan install . -s build_type=Debug -s arch=x86 -if build_debug --build=enet --build=glfw --build=imgui
conan install . -s build_type=Release -s arch=x86 -if build_release --build=enet --build=glfw --build=imgui

cmake_minimum_required(VERSION 3.26)
project(BlackHole)

set(CMAKE_CXX_STANDARD 20)

add_executable(BlackHole src/main.cpp
        src/shader/shader.cpp
        src/shader/shader.h
        src/glad/glad.c
        src/log/easylogging++.cc
        src/stb_image.h
        src/GLFW/glfw3.h
)

#include_directories("C:/Users/domin/Downloads/glfw-3.3.8.bin.WIN64.zip/glfw-3.3.8.bin.WIN64/include/GLFW")
target_link_libraries(BlackHole "C:/Users/domin/CLionProjects/VoxelBlackHoleS/cmake-build-debug/glfw3.dll")

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/src/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(BlackHole copy_assets)

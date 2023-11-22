cmake_minimum_required(VERSION 3.26)
project(BlackHole)

set(CMAKE_CXX_STANDARD 20)

add_executable(BlackHole src/main.cpp
        src/shader/shader.cpp
        src/shader/shader.h
        src/glad/glad.c
        src/log/easylogging++.cc
        src/stb_image.h
)

target_link_libraries(BlackHole glfw)

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/src/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(BlackHole copy_assets)

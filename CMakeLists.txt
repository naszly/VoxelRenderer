cmake_minimum_required(VERSION 3.25)
project(VoxelRenderer)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3)
find_package(GLEW)

include(cmake/CPM.cmake)

if (NOT glfw3_FOUND)
    CPMAddPackage(
            NAME glfw
            GITHUB_REPOSITORY glfw/glfw
            GIT_TAG 3.3.8
            OPTIONS
            "GLFW_BUILD_EXAMPLES OFF"
            "GLFW_BUILD_TESTS OFF"
            "GLFW_BUILD_DOCS OFF"
            "GLFW_INSTALL OFF"
    )
endif ()

if (NOT GLEW_FOUND)
    CPMAddPackage(
            NAME GLEW
            GITHUB_REPOSITORY Perlmint/glew-cmake
            GIT_TAG glew-cmake-2.2.0
            OPTIONS
            "ONLY_LIBS ON"
    )
endif ()

add_executable(${PROJECT_NAME} main.cpp shader.h shader.cpp buffer.h vertex_array.cpp vertex_array.h world/voxel.h world/chunk.h world/world.h player_controller.cpp player_controller.h)

target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw GLEW)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

# custom target to copy shaders to build directory
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders)

add_dependencies(${PROJECT_NAME} copy_shaders)
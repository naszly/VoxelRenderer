cmake_minimum_required(VERSION 3.22)
project(VoxelRenderer)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3)
find_package(GLEW)
find_package(glm)

include(cmake/CPM.cmake)

if (NOT glfw3_FOUND)
    message(STATUS "glfw not found, downloading...")
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
    message(STATUS "GLEW not found, downloading...")
    CPMAddPackage(
            NAME GLEW
            GITHUB_REPOSITORY Perlmint/glew-cmake
            GIT_TAG glew-cmake-2.2.0
            OPTIONS
            "ONLY_LIBS ON"
            "glew-cmake_BUILD_SHARED OFF"
            "glew-cmake_BUILD_STATIC ON"
    )
    add_library(GLEW::GLEW ALIAS libglew_static)
endif ()

if (NOT glm_FOUND)
    message(STATUS "glm not found, downloading...")
    CPMAddPackage(
            NAME glm
            GITHUB_REPOSITORY g-truc/glm
            GIT_TAG 0.9.9.8
    )
endif ()

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.h)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw GLEW::GLEW glm)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

# custom target to copy shaders to build directory
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders)

add_dependencies(${PROJECT_NAME} copy_shaders)

# custom target to copy textures to build directory
add_custom_target(copy_textures ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/textures
        ${CMAKE_BINARY_DIR}/textures)

add_dependencies(${PROJECT_NAME} copy_textures)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.10)

SET(PROJECT_NAME "Lemonade")

# Project name and language
project(${PROJECT_NAME} CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)

# add dependencies
include(cmake/CPM.cmake)

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Find Vulkan package
find_package(Vulkan REQUIRED)

# SDL3 Package
CPMAddPackage(
    NAME sdl
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.2.8/SDL3-3.2.8.tar.gz
    URL_HASH SHA256=13388fabb361de768ecdf2b65e52bb27d1054cae6ccb6942ba926e378e00db03
)

# SDL Image
CPMAddPackage(
    NAME sdl_image
    URL https://github.com/libsdl-org/SDL_image/releases/download/release-3.2.4/SDL3_image-3.2.4.tar.gz
    URL_HASH SHA256=a725bd6d04261fdda0dd8d950659e1dc15a8065d025275ef460d32ae7dcfc182
)

# GLM
CPMAddPackage("gh:g-truc/glm#1.0.1")

# JSON
CPMAddPackage(
    NAME JSON    
    GIT_REPOSITORY https://github.com/nlohmann/Json.git
    GIT_TAG v3.11.3
)

# DX Shader Compiler - HLSL -> SPIR-V
find_library(DXC_LIB dxcompiler)

# GLEW
##CPMAddPackage(
##  NAME GLEW
##  GIT_REPOSITORY https://github.com/iauns/cpm-glew.git
##  GIT_TAG        1.0.1
##)

# Add the include directory as the root for the source directory
include_directories("./source")

# Add library includes
#include_directories("./libraries/include")

# Find all source files recursively in the source directory
file(GLOB_RECURSE SOURCES "./source/*.cpp")

# Add executable or library (depending on your project type)
# add_executable(MyProject ${SOURCES})

# If you're creating a library, use add_library instead of add_executable
add_library(${PROJECT_NAME} ${SOURCES})

# Include library headers 
target_include_directories(${PROJECT_NAME} PUBLIC 
    #${GLEW_SOURCE_DIR}/glew/include
    ${JSON_SOURCE_DIR}/include
)

#message("Json includes located at: '${JSON_SOURCE_DIR}/include'")

# Include headers
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)

# Link third party dependencies.
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    glm::glm
    OpenGL::GL
    Vulkan::Vulkan
    dxcompiler
    #${Vulkan_LIBRARIES}
    #Trying to stick to modern GL 3.x + glew/ glu should not be required
    #glew
    CitrusCore
)
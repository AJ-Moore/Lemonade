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

# Add SPIRV-Headers first (needed by SPIRV-Tools)
CPMAddPackage(
  NAME SPIRV-Headers
  GITHUB_REPOSITORY KhronosGroup/SPIRV-Headers
  GIT_TAG 09913f088a1197aba4aefd300a876b2ebbaa3391
)

# Then SPIRV-Tools, and make sure to disable its tests
CPMAddPackage(
  NAME SPIRV-Tools
  GITHUB_REPOSITORY KhronosGroup/SPIRV-Tools
  GIT_TAG v2025.1
  OPTIONS
    "SPIRV_TOOLS_BUILD_STATIC ON"
    "SPIRV_TOOLS_BUILD_TESTS OFF"
    "SPIRV_WERROR OFF"
)

# glslang
CPMAddPackage(
  NAME glslang
  GITHUB_REPOSITORY KhronosGroup/glslang
  GIT_TAG main
  OPTIONS
    "ENABLE_GLSLANG_BINARIES OFF"
    "ENABLE_HLSL ON"
    "ENABLE_OPT OFF"
    "ENABLE_CTEST OFF"
)

# Shaderc glsl -> spirv
CPMAddPackage(
  NAME shaderc
  GITHUB_REPOSITORY google/shaderc
  GIT_TAG main
  OPTIONS
    "SHADERC_SKIP_TESTS ON"
    "SHADERC_ENABLE_TESTS OFF"
    "SHADERC_ENABLE_WERROR_COMPILE OFF"
    "ENABLE_SPIRV_TOOLS_INSTALL OFF"
    "BUILD_TESTING OFF"
)

CPMAddPackage(
  NAME assimp
  GITHUB_REPOSITORY assimp/assimp
  GIT_TAG v6.0.2
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
    assimp
    OpenGL::GL
    Vulkan::Vulkan
    #dxcompiler
    shaderc
    #${Vulkan_LIBRARIES}
    #Trying to stick to modern GL 3.x + glew/ glu should not be required
    #glew
    CitrusCore
)
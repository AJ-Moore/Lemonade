#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define LEMONADE_API __declspec(dllexport)
#else
#define LEMONADE_API __attribute__((visibility("default")))
#endif

#define RENDERER_OPENGL
#define RENDERER_VULKAN
//#define RENDERER_DX12

// C++ Standard Library Includes
#include <string>
#include <memory>
#include <vector>

// Third Party Includes
#include <glm/glm.hpp>
#include <glm/ext.hpp>

// Include citrus core 
#include <CitrusCore.h>

// SDL 
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#if defined(RENDERER_VULKAN)
#include <vulkan/vulkan.h>
#endif

#if defined(RENDERER_OPENGL)
#include <GL/gl.h>
#endif

#if defined(RENDERER_OPENGL) || defined(RENDERER_VULKAN) || defined(RENDERER_DX12)
#define USING_SDL
#endif

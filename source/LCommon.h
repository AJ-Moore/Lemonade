#pragma once

#ifdef _WIN32 || _WIN64
#define LEMONADE_API __declspec(dllexport)
#else
#define LEMONADE_API __attribute__((visibility("default")))
#endif

#define RENDERER_OPENGL

// C++ Standard Library Includes
#include <string>
#include <memory>
#include <vector>

// Third Party Includes
#include <glm/glm.hpp>
#include <glm/ext.hpp>

// Include citrus core 
#include <CitrusCore.h>
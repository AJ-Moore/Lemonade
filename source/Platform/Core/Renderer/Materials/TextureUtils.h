#pragma once

#include <CCCommon.h>

struct Colour
{
	Colour(uint8 r, uint8 g, uint8 b, uint8 a) : r(r), g(g), b(b), a(a) {}
	uint8 r = 0;
	uint8 g = 0;
	uint8 b = 0;
	uint8 a = 255;
};

enum class TextureFilter
{
	NearestNeighbour,
	Linear,
	None
};

enum class TextureClamp
{
	Clamp,
	Repeat,
	None
};

enum class TextureFormat
{
	LEMONADE_UNKNOWN = 0,
	LEMONADE_RGBA8888 = 1,
	LEMONADE_BGRA8888 = 2,
	LEMONADE_RGB888 = 3,
	LEMONADE_BGR888 = 4,
};
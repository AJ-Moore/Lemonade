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
	LEMONADE_FORMAT_UNKNOWN = 0,
	LEMONADE_FORMAT_RGBA8888 = 1,
	LEMONADE_FORMAT_BGRA8888 = 2,
	LEMONADE_FORMAT_RGB888 = 3,
	LEMONADE_FORMAT_BGR888 = 4,
	LEMONADE_FORMAT_ABGR8888 = 5,
	LEMONADE_FORMAT_R8 = 6,
	LEMONADE_FORMAT_R16 = 7,
};
#pragma once

#include <Common.h>

namespace CraftyBlocks {

	/** Flags used for identifing primitive mode information - Matches OpenGL equivilent. */
	enum class PrimitiveMode : uint32 {
		POINTS = 0x1,
		LINES = 0x2,
		LINE_STRIP = 0x4,
		LINE_LOOP = 0x8,
		TRIANGLES = 0x10,
		TRIANGLE_STRIP = 0x20,
		TRIANGLE_FAN = 0x40
	};

}

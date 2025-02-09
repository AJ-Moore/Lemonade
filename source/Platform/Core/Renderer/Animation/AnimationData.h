#pragma once
#include <Common.h>

struct AnimationData
{
	AnimationData() = default;
	int ParentBoneId = -1;
	int BoneId = 0;
	float Duration;
	float TicksPerSecond;
	alignas(4) glm::mat4 OffsetMatrix;
	int RotationKeyCount;
	int PositionKeyCount;
	int ScaleKeyCount;
};

#pragma once

#include <LCommon.h>
#include <memory>
#include <vector>
#include <Platform/Core/Renderer/Animation/BoneAnim.h>

namespace Lemonade
{
	class LAnimation
	{
	public:
		LAnimation(std::string name, double duration, double ticksPerSecond);
		void AddBoneAnimation(std::shared_ptr<LBoneAnim> animation) { m_boneAnimations.push_back(animation); }
		const std::vector<std::shared_ptr<LBoneAnim>>& getBoneAnimations() { return m_boneAnimations; }
		double GetDuration() { return m_duration; }
		double GetTicksPerSecond() { return m_ticksPerSecond; }
	private:
		// Name of mesh to be animated, name is passed to shader to identify mesh.
		std::string m_name;
		double m_duration = 0;
		double m_ticksPerSecond = 0;

		std::vector<std::shared_ptr<LBoneAnim>> m_boneAnimations;
	};
}
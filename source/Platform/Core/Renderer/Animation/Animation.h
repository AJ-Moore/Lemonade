#pragma once

#include <Common.h>
#include <memory>
#include <vector>
#include <Renderer/BoneAnim.h>

namespace CraftyBlocks
{
	class Animation
	{
	public:
		Animation(std::string name, double duration, double ticksPerSecond);
		void AddBoneAnimation(std::shared_ptr<BoneAnim> animation) { m_boneAnimations.push_back(animation); }
		const std::vector<std::shared_ptr<BoneAnim>>& getBoneAnimations() { return m_boneAnimations; }
		double GetDuration() { return m_duration; }
		double GetTicksPerSecond() { return m_ticksPerSecond; }
	private:
		// Name of mesh to be animated, name is passed to shader to identify mesh.
		std::string m_name;
		double m_duration = 0;
		double m_ticksPerSecond = 0;

		std::vector<std::shared_ptr<BoneAnim>> m_boneAnimations;
	};
}
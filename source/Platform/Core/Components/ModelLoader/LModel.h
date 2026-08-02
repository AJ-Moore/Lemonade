#pragma once
#include <Containers/IndexedVector.h>
#include <Util/UID.h>
#include <Platform/Core/Components/ModelLoader/LModelResource.h>
#include <LCommon.h>
#include <Platform/Core/Components/RenderComponent.h>
#include <Resources/ResourceHandle.h>
#include <Platform/Core/Components/LMeshRenderer.h>
#include <Platform/Core/Renderer/Materials/Material.h>
#include <glm/fwd.hpp>
#include <unordered_map>

namespace Lemonade
{
	class LEMONADE_API LModel : public RenderComponent
	{
		struct LEMONADE_API LModelAnim 
		{
			CitrusCore::UID AnimationId;
			uint32 AnimationIndex = 0;	
		};
	public:
		LModel() = default;
        LModel(CitrusCore::ResourcePtr<Lemonade::LModelResource> modelResource);
		virtual bool Init() override;
		virtual void Update() override;
		virtual void Render() override;

		void SetModelResource(CitrusCore::ResourcePtr<Lemonade::LModelResource> modelResource);

		CitrusCore::UID LoadAnimation(const std::string& modelPath, const std::string& animationName, uint32 animationIndex = 0);
		void PlayAnimation(std::string animationName);

		glm::vec3 GetRootMotionDelta() const { return m_model->GetResource()->GetRootMotionDelta(); }
		glm::quat GetRootMotionRotation() const { return m_model->GetResource()->GetRootMotionRotation(); }

		void SetApplyRootMotion(bool bApply) { m_bApplyRootMotion = bApply; }
	private:
		bool m_bApplyRootMotion = true;
		CitrusCore::ResourcePtr<LModelResource> m_model;

		// Animations 
		CitrusCore::IndexedVector<CitrusCore::UID, CitrusCore::ResourcePtr<LModelResource>> m_animations;

		/// Associate a string identifier with our animation reosurce and animation index to be played.
		std::unordered_map<std::string, LModelAnim> m_animationData;
	};
}

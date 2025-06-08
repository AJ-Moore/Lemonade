#include "Resources/ResourceHandle.h"
#include <Platform/Core/Components/LMeshRenderer.h>
#include <LCommon.h>
#include <memory>

namespace Lemonade 
{
    class LEMONADE_API LSpriteRenderer : public LMeshRenderer {
		friend class LSpriteAnimator;
	public:
        LSpriteRenderer() = delete;
        LSpriteRenderer(CitrusCore::ResourcePtr<Material> material);
		virtual ~LSpriteRenderer() = default;
		virtual bool Init() override;

		//std::shared_ptr<LSpriteAnimator> GetAnimator() const { return this->m_animator; }
		//virtual void SetAnimator(LSpriteAnimator* Animator);
		virtual void SetMaterial(CitrusCore::ResourcePtr<Material> Mat) override;
		virtual void SetFrame(uint32 frameIndex);

	protected:
		std::shared_ptr<std::vector<glm::vec2>> m_uvs;
		//std::shared_ptr<LSpriteAnimator> m_animator = nullptr; 
		uint32 m_currentFrame = 0;
	};
}
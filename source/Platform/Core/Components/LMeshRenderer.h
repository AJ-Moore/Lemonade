#include <Platform/Core/Renderer/Geometry/PrimitiveMode.h>
#include <Resources/ResourceHandle.h>
#include <LCommon.h>
#include <Platform/Core/Components/RenderComponent.h>
#include <memory>

namespace Lemonade
{
	class LEMONADE_API LMeshRenderer : public RenderComponent
	{
	public:
		virtual ~LMeshRenderer(){}

		virtual bool Init();
		virtual void Update();
		virtual void Render();
		virtual void Unload();

		virtual void SetDrawMode(PrimitiveMode mode);
		virtual void SetMaterial(CitrusCore::ResourcePtr<Material>);
		void AssignMesh(std::shared_ptr<Mesh>);
		std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }
		virtual void setDirty();

	protected:
		std::shared_ptr<Mesh> m_mesh = nullptr;
		CitrusCore::ResourcePtr<Material> m_material = nullptr;
	};

}
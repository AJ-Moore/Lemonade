#pragma once

#include <LCommon.h>

namespace Lemonade
{
	class LEMONADE_API LObject
	{
	public:
		virtual ~LObject() {}

		void SetName(const std::string& name) { m_name = name; }
		const std::string& GetName() const { return m_name; }

		CitrusCore::UID UID;

	protected:
		virtual bool Init() = 0;
		virtual void Update() = 0;
		virtual void Render() {}

	private:
		std::string m_name;
	};
}
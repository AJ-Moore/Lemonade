#pragma once

namespace Lemonade
{
	class IManagedObject {
	public:
		virtual bool Init() = 0; 
		virtual void Unload() = 0; 
		virtual void Update() = 0; 
		virtual void Render() = 0; 
	};
}
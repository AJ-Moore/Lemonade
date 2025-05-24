#include <Platform/Core/Services/GraphicsServices.h>
#include <LemonadeRE.h>

namespace Lemonade
{
    void LemonadeRE::Init(){
        GraphicsServices::GetInstance().Init();
    }
    void LemonadeRE::Update(){
        GraphicsServices::GetInstance().Update();
        GraphicsServices::GetInstance().Render();
    }
    void LemonadeRE::Unload(){
        GraphicsServices::GetInstance().Unload();
    }

}
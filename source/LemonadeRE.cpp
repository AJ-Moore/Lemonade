#include "Platform/Core/Services/Services.h"
#include <LemonadeRE.h>

namespace Lemonade
{
    void LemonadeRE::Init(){
        Services::GetInstance().Init();
    }
    void LemonadeRE::Update(){
        Services::GetInstance().Update();
        Services::GetInstance().Render();
    }
    void LemonadeRE::Unload(){
        Services::GetInstance().Unload();
    }

}
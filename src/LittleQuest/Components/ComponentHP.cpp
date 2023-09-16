#include "ComponentHP.h"

#include <System/Scene.h>
#include <System/Component/Component.h>
#include <System/Component/ComponentModel.h>
#include <System/Utils/IniFileLib.h>

namespace LittleQuest
{
void ComponentHP::Init()
{
    __super::Init();
}

void ComponentHP::Update()
{
    __super::Update();
}

void ComponentHP::GUI()
{
    __super::GUI();
}

BP_COMPONENT_IMPL(ComponentHP, u8"HPコンポーネント");
}   // namespace LittleQuest

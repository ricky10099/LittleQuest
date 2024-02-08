#include "Rock3.h"

#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(Rock3, "LittleQuest/Rock3");

Rock3Ptr Rock3::Create(const float3& pos) {
    auto obj = Scene::CreateObjectPtr<Rock3>();
    obj->SetName("Rock3");
    obj->SetTranslate(pos);

    return obj;
}

bool Rock3::Init() {
    AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj004.mv1");
    AddComponent<ComponentCollisionModel>()->AttachToModel();
    return Super::Init();
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Rock3)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Rock3)

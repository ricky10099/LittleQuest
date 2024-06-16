#include "Rock2.h"

#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
Rock2Ptr Rock2::Create(const float3& pos) {
    auto obj = Scene::CreateObjectPtr<Rock2>();
    obj->SetName("Rock2");
    obj->SetTranslate(pos);

    return obj;
}

bool Rock2::Init() {
    AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj005.mv1");
    AddComponent<ComponentCollisionModel>()->AttachToModel();
    return Super::Init();
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Rock2)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Rock2)

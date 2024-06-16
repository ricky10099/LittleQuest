#include "Rock1.h"

#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
Rock1Ptr Rock1::Create(const float3& pos) {
    auto obj = Scene::CreateObjectPtr<Rock1>();
    obj->SetName("Rock1");
    obj->SetTranslate(pos);

    return obj;
}

bool Rock1::Init() {
    AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj002.mv1");
    AddComponent<ComponentCollisionModel>()->AttachToModel();
    return Super::Init();
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Rock1)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Rock1)

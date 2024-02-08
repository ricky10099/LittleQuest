#include "BrokenHouse.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
BP_OBJECT_IMPL(BrokenHouse, "LittleQuest/BrokenHouse");
BrokenHousePtr BrokenHouse::Create(std::string name, const float3& pos) {
    auto obj = Scene::CreateObjectPtr<BrokenHouse>();
    obj->SetName(name);
    obj->SetTranslate(pos);
    obj->AddComponent<ComponentModel>("data/LittleQuest/Model/BrokenHouse/BrokenHouse.mv1");

    obj->m_pBox = Scene::CreateObjectPtr<Object>("BrokenHouseBox");
    obj->m_pBox->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009_c.mv1");
    obj->m_pBox->SetTranslate(pos + float3{-3, 0, 0});
    obj->m_pBox->SetRotationAxisXYZ({0, 90, 0});
    obj->m_pBox->SetScaleAxisXYZ({1, 1, 16});
    obj->m_pBox->AddComponent<ComponentCollisionModel>()->AttachToModel();

    return obj;
}
void BrokenHouse::Update() {
    if(!m_pBox->GetStatus(StatusBit::NoDraw)) {
        m_pBox->SetStatus(StatusBit::NoDraw, true);
    }
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::BrokenHouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::BrokenHouse)

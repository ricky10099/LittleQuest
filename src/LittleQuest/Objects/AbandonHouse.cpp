#include "AbandonHouse.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
//------------------------------------------------------------
// オブジェクトを生成します。
//------------------------------------------------------------
AbandonHousePtr AbandonHouse::Create(std::string name, const float3& pos) {
    auto obj = Scene::CreateObjectPtr<AbandonHouse>();
    obj->SetName(name);
    obj->SetTranslate(pos);
    obj->AddComponent<ComponentModel>("data/LittleQuest/Model/AbandonHouse/AbandonHouse.mv1");

    obj->m_pBox = Scene::CreateObjectPtr<Object>("AbandonHouseBox");
    obj->m_pBox->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009_c.mv1");
    obj->m_pBox->SetTranslate(pos + float3{-5, 0, -5});
    obj->m_pBox->SetRotationAxisXYZ({0, 90, 0});
    obj->m_pBox->SetScaleAxisXYZ({1, 1, 19});
    obj->m_pBox->AddComponent<ComponentCollisionModel>()->AttachToModel();

    return obj;
}

//------------------------------------------------------------
// 更新処理を行います。
//------------------------------------------------------------
void AbandonHouse::Update() {
    if(!m_pBox->GetStatus(StatusBit::NoDraw)) {
        m_pBox->SetStatus(StatusBit::NoDraw, true);
    }
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::AbandonHouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::AbandonHouse)

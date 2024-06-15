#include "DestroyedHouse.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
DestroyedHousePtr DestroyedHouse::Create(std::string name, const float3& pos) {
    auto obj = Scene::CreateObjectPtr<DestroyedHouse>();
    obj->SetName(name);
    obj->SetTranslate(pos);
    obj->AddComponent<ComponentModel>("data/LittleQuest/Model/DestroyedHouse/DestroyedHouse.mv1");

    obj->m_pBox = Scene::CreateObjectPtr<Object>("DestroyedHouseBox");
    obj->m_pBox->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009_c.mv1");
    obj->m_pBox->SetTranslate(pos + float3{-6, 0, -2});
    obj->m_pBox->SetRotationAxisXYZ({0, 90, 0});
    obj->m_pBox->SetScaleAxisXYZ({0.9, 1, 12});
    obj->m_pBox->AddComponent<ComponentCollisionModel>()->AttachToModel();

    return obj;
}

bool DestroyedHouse::Init() {
    return Super::Init();
}

void DestroyedHouse::Update() {
    if(!m_pBox->GetStatus(StatusBit::NoDraw)) {
        m_pBox->SetStatus(StatusBit::NoDraw, true);
    }
}

void DestroyedHouse::LateDraw() {}

void DestroyedHouse::GUI() {
    Super::GUI();
}

void DestroyedHouse::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)    // override
{
    Super::OnHit(hitInfo);
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::DestroyedHouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::DestroyedHouse)

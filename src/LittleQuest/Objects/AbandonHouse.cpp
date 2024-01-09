#include "AbandonHouse.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(AbandonHouse, "LittleQuest/AbandonHouse");

AbandonHousePtr AbandonHouse::Create(std::string name, const float3& pos) {
    auto obj = Scene::CreateObjectPtr<AbandonHouse>();
    obj->SetName(name);
    obj->SetTranslate(pos);

    return obj;
}

bool AbandonHouse::Init()    // override
{
    AddComponent<ComponentModel>("data/LittleQuest/Model/AbandonHouse/AbandonHouse.mv1");
    AddComponent<ComponentCollisionModel>()->AttachToModel();
    return Super::Init();
}

void AbandonHouse::Update()    // override
{}

// 基本描画の後に処理します
void AbandonHouse::LateDraw()    // override
{}

void AbandonHouse::GUI()    // override
{
    Super::GUI();
}

void AbandonHouse::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)    // override
{
    Super::OnHit(hitInfo);
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::AbandonHouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::AbandonHouse)

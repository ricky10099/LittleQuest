#include "DestroyedHouse.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(DestroyedHouse, "LittleQuest/DestroyedHouse");

DestroyedHousePtr DestroyedHouse::Create(std::string name, const float3& pos) {
    auto obj = Scene::CreateObjectPtr<DestroyedHouse>();
    obj->SetName(name);
    obj->SetTranslate(pos);
    obj->AddComponent<ComponentModel>("data/LittleQuest/Model/DestroyedHouse/DestroyedHouse.mv1");

    return obj;
}

bool DestroyedHouse::Init()    // override
{
    auto box = Scene::CreateObjectPtr<Object>("DestroyedHouseBox");
    box->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009_c.mv1");
    box->SetTranslate(GetTranslate() + float3{-6, 0, -2});
    box->SetRotationAxisXYZ({0, 90, 0});
    box->SetScaleAxisXYZ({0.9, 1, 12});
    box->AddComponent<ComponentCollisionModel>()->AttachToModel();
    box->SetStatus(StatusBit::NoDraw, true);

    return Super::Init();
}

void DestroyedHouse::Update()    // override
{}

// 基本描画の後に処理します
void DestroyedHouse::LateDraw()    // override
{}

void DestroyedHouse::GUI()    // override
{
    Super::GUI();
}

void DestroyedHouse::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)    // override
{
    Super::OnHit(hitInfo);
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::DestroyedHouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::DestroyedHouse)

#include "BrokenHouse.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(BrokenHouse, "LittleQuest/BrokenHouse");

BrokenHousePtr BrokenHouse::Create(std::string name, const float3& pos) {
    auto obj = Scene::CreateObjectPtr<BrokenHouse>();
    obj->SetName(name);
    obj->SetTranslate(pos);
    obj->AddComponent<ComponentModel>("data/LittleQuest/Model/BrokenHouse/BrokenHouse.mv1");

    return obj;
}

bool BrokenHouse::Init()    // override
{
    auto box = Scene::CreateObjectPtr<Object>("BrokenHouseBox");
    box->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009_c.mv1");
    box->SetTranslate(GetTranslate() + float3{-3, 0, 0});
    box->SetRotationAxisXYZ({0, 90, 0});
    box->SetScaleAxisXYZ({1, 1, 16});
    box->AddComponent<ComponentCollisionModel>()->AttachToModel();
    box->SetStatus(StatusBit::NoDraw, true);

    return Super::Init();
}

void BrokenHouse::Update()    // override
{}

// 基本描画の後に処理します
void BrokenHouse::LateDraw()    // override
{}

void BrokenHouse::GUI()    // override
{
    Super::GUI();
}

void BrokenHouse::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)    // override
{
    Super::OnHit(hitInfo);
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::BrokenHouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::BrokenHouse)

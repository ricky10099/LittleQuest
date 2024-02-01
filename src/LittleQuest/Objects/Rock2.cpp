#include "Rock2.h"

#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(Rock2, "LittleQuest/Rock2");

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

void Rock2::Update() {}

// 基本描画の後に処理します
void Rock2::LateDraw() {}

void Rock2::GUI() {
    Super::GUI();
}

void Rock2::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    Super::OnHit(hitInfo);
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Rock2)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Rock2)

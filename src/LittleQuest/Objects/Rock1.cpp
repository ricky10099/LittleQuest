#include "Rock1.h"

#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(Rock1, "LittleQuest/Rock1");

Rock1Ptr Rock1::Create(const float3& pos) {
    auto obj = Scene::CreateObjectPtr<Rock1>();
    obj->SetName("Rock1");
    obj->SetTranslate(pos);

    return obj;
}

bool Rock1::Init()    // override
{
    AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj002.mv1");
    AddComponent<ComponentCollisionModel>()->AttachToModel();
    return Super::Init();
}

void Rock1::Update()    // override
{}

// 基本描画の後に処理します
void Rock1::LateDraw()    // override
{}

void Rock1::GUI()    // override
{
    Super::GUI();
}

void Rock1::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)    // override
{
    Super::OnHit(hitInfo);
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Rock1)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Rock1)

#include "Sword.h"
#include "Camera.h"
#include "Enemy.h"
#include "Player.h"

#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentAttachModel.h>

namespace LittleQuest
{

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(Sword, "LittleQuest/Sword");

SwordPtr Sword::Create(std::string name, const float3& pos)
{
    // 箱の作成
    auto sword = Scene::CreateObjectPtr<Sword>();
    sword->SetName(name);
    // posの位置に設定
    sword->SetTranslate(pos);

    return sword;
}

bool Sword::Init()   // override
{
    Super::Init();

    // モデルコンポーネント(0.08倍)
    // モデルコンポーネント(0.08倍)
    //auto model = AddComponent<ComponentModel>("data/LittleQuest/Model/Sword/Sword.mv1");
    //model->SetScaleAxisXYZ({0.1f, 0.06f, 0.1f});
    //model->SetScaleAxisXYZ({0.05f});   //

    //// コリジョン(カプセル)
    //auto col = AddComponent<ComponentCollisionLine>();   //
    //col->SetTranslate({0, 0, 0});
    //col->SetLine(float3{0, 0, 0}, float3{0, 110, 1});
    //col->SetCollisionGroup(ComponentCollision::CollisionGroup::WEAPON);

    //this->SetAtkVal(1);

    return true;
}

void Sword::Update()   // override
{
}

// 基本描画の後に処理します
void Sword::LateDraw()   // override
{
    printfDx("\nSword isAttack: %i", isAttack);
}

void Sword::GUI()   // override
{
    Super::GUI();
}

void Sword::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)   // override
{
    //// 次のownerのオブジェクトと当たった!
    //auto  ownerPtr = hitInfo.hit_collision_->GetOwnerPtr();
    //auto* owner    = hitInfo.hit_collision_->GetOwner();
    //printfDx("\nSword Hit:%s", ownerPtr->GetName().data());

    //if(ownerPtr->GetName() == "Enemy" && isAttack) {
    //    if(auto enemy = dynamic_cast<Enemy*>(owner))
    //        enemy->Damaged(this->atkVal);
    //}
    //hitInfo.hit_collision_->GetOwner();
    ////if(owner->)

    Super::OnHit(hitInfo);
}

void Sword::Attack()
{
    isAttack = true;
}

void Sword::FinishAttack()
{
    isAttack = false;
}

void Sword::SetAtkVal(int atkVal)
{
    this->atkVal = atkVal;
}

}   // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Sword)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Sword)

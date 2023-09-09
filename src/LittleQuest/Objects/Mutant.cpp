#include "Mutant.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>

namespace LittleQuest
{

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(Mutant, "LittleQuest/Mutant");

MutantPtr Mutant::Create(const float3& pos, const float3& front)
{
    // 箱の作成
    auto enemy = Scene::CreateObjectDelayInitialize<Mutant>();
    enemy->SetName("Mutant");

    // vecの方向に向ける
    auto mat = HelperLib::Math::CreateMatrixByFrontVector(front);
    enemy->SetMatrix(mat);

    // posの位置に設定
    enemy->SetTranslate(pos);

    return enemy;
}

bool Mutant::Init()   // override
{
    Super::Init();

    // モデルコンポーネント(0.08倍)
    auto model = AddComponent<ComponentModel>("data/LittleQuest/Model/Mutant/Mutant.mv1");

    model->SetScaleAxisXYZ({0.05f});   //

    model->SetAnimation({
        {  "idle",    "data/LittleQuest/Anim/MutantIdle.mv1", 0, 1.0f},
        {   "run",     "data/LittleQuest/Anim/MutantRun.mv1", 0, 1.0f},
        {"attack", "data/LittleQuest/Anim/MutantSwiping.mv1", 0, 1.0f},
        {"getHit",     "data/LittleQuest/Anim/HitToBody.mv1", 0, 1.0f},
        {   "die",   "data/LittleQuest/Anim/MutantDying.mv1", 0, 1.0f}
    });
    model->PlayAnimation("idle", true);

    // コリジョン(カプセル)
    auto col = AddComponent<ComponentCollisionCapsule>();   //
    col->SetTranslate({0, 0, 0});
    col->SetRadius(2.5);
    col->SetHeight(10);
    col->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);
    col->UseGravity();

    setHP(200);

    startPoint = this->GetTranslate() - float3{50, 0, 0};
    patrolPoint.push_back(startPoint);

    endPoint = this->GetTranslate() + float3{50, 0, 0};
    patrolPoint.push_back(endPoint);

    patrolIndex = 1;
    goal        = patrolPoint[patrolIndex];

    state = EnemyState::PATROL;

    return true;
}

void Mutant::Update()   // override
{
    Super::Update();
}

// 基本描画の後に処理します
void Mutant::LateDraw()   // override
{
    Super::LateDraw();
}

void Mutant::GUI()   // override
{
    Super::GUI();
}

void Mutant::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)   // override
{
    Super::OnHit(hitInfo);
}

void Mutant::Patrol(float3& move)
{
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        modelPtr->PlayAnimationNoSame("run", true);
    }

    Super::Patrol(move);
}

// void Mutant::Attack()
//{
//     if(auto modelPtr = GetComponent<ComponentModel>()) {
//         modelPtr->PlayAnimationNoSame("attack");
//     }
// }

void Mutant::GetHit(int damage)
{
    Super::GetHit(damage);
}

void Mutant::Die()
{
    Super::Die();
}

}   // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Mutant)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Mutant)

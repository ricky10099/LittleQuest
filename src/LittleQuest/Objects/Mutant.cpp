#include "Mutant.h"
#include "../Components/ComponentHP.h"

#include <System/Component/Component.h>
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>

namespace LittleQuest {

    //! @brief Editor上でのCreateObject用の設定
    //! @detail BP_OBJECT_TYPEとセットで用意する
    BP_OBJECT_IMPL(Mutant, "LittleQuest/Mutant");

    MutantPtr Mutant::Create(const float3& pos, bool isPatrol) {
        // 箱の作成
        auto enemy = Scene::CreateObjectDelayInitialize<Mutant>();
        enemy->SetName("Mutant");

        // vecの方向に向ける
        const float3& front = {0, 0, 1};
        auto mat            = HelperLib::Math::CreateMatrixByFrontVector(front);
        enemy->SetMatrix(mat);

        // posの位置に設定
        enemy->SetTranslate(pos);

        auto hp = enemy->AddComponent<ComponentHP>();
        hp->SetHP(200);

        enemy->spawnPos   = pos;
        enemy->spawnPos.y = 0;
        enemy->isPatrol   = isPatrol;

        return enemy;
    }

    bool Mutant::Init()    // override
    {
        Super::Init();

        // モデルコンポーネント(0.08倍)
        auto model = AddComponent<ComponentModel>("data/LittleQuest/Model/Mutant/Mutant.mv1");

        model->SetScaleAxisXYZ({0.05f});    //

        model->SetAnimation({{"idle", "data/LittleQuest/Anim/MutantIdle.mv1", 0, 1.0f},
                             {"walk", "data/LittleQuest/Anim/MutantWalking.mv1", 0, 1.0f},
                             {"run", "data/LittleQuest/Anim/MutantRun.mv1", 0, 1.0f},
                             {"attack", "data/LittleQuest/Anim/MutantSwiping.mv1", 0, 1.0f},
                             {"getHit", "data/LittleQuest/Anim/HitToBody.mv1", 0, 2.0f},
                             {"die", "data/LittleQuest/Anim/MutantDying.mv1", 0, 1.0f}});
        model->PlayAnimation("idle", true);

        // コリジョン(カプセル)
        auto col = AddComponent<ComponentCollisionCapsule>();    //
        col->SetTranslate({0, 0, 0});
        col->SetRadius(2.5);
        col->SetHeight(10);
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);
        col->UseGravity();

        // コリジョン(武器)
        auto colWeapon = AddComponent<ComponentCollisionCapsule>();
        colWeapon->AttachToModel("mixamorig:LeftHand");
        colWeapon->SetTranslate({0, 0, 0});
        colWeapon->SetRadius(0.5);
        colWeapon->SetHeight(4);
        colWeapon->SetRotationAxisXYZ({-10, 0, 20});
        colWeapon->SetCollisionGroup(ComponentCollision::CollisionGroup::WEAPON);
        colWeapon->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);
        colWeapon->Overlap((u32)ComponentCollision::CollisionGroup::PLAYER);
        colWeapon->SetName("MutantWeapon");

        return true;
    }

    void Mutant::Update()    // override
    {
        Super::Update();
    }

    // 基本描画の後に処理します
    void Mutant::LateDraw()    // override
    {
        Super::LateDraw();

        DrawSphere3D(VGet(spawnPos.x, spawnPos.y, spawnPos.z), 10, 2, GetColor(255, 255, 0), GetColor(255, 255, 255), TRUE);
    }

    void Mutant::GUI()    // override
    {
        Super::GUI();
    }

    void Mutant::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)    // override
    {
        // 武器の衝突判定
        if (hitInfo.collision_->GetName() == "MutantWeapon") {
            // attack anim 1.3s からダメージ発生
            if (auto modelPtr = GetComponent<ComponentModel>()) {
                if (modelPtr->GetPlayAnimationName() == "attack") {
                    if (modelPtr->GetAnimationTime() > 1.3f) {
                        isAttack = true;
                    }
                }
            }

            auto* owner = hitInfo.hit_collision_->GetOwner();
            if (isAttack) {
                if (auto player = dynamic_cast<Player*>(owner)) {
                    if (!isHitPlayer) {
                        isHitPlayer = true;
                        player->GetHit(30);
                    }
                }
            }
        }

        Super::OnHit(hitInfo);
    }

    void Mutant::BackToInitialPosition(float3& move) {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            modelPtr->PlayAnimationNoSame("run", true);
        }

        Super::BackToInitialPosition(move);
    }

    void Mutant::Patrol(float3& move) {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            modelPtr->PlayAnimationNoSame("walk", true);
        }

        Super::Patrol(move);
    }

    void Mutant::GetHit(int damage) {
        Super::GetHit(damage);
    }

    void Mutant::Die() {
        Super::Die();
    }

}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Mutant)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Mutant)

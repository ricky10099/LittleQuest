#include "Zombie.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>

#include "Camera.h"

namespace LittleQuest {

    //! @brief Editor上でのCreateObject用の設定
    //! @detail BP_OBJECT_TYPEとセットで用意する
    BP_OBJECT_IMPL(Zombie, "LittleQuest/Zombie");

    ZombiePtr Zombie::Create(const float3& pos, const float3& front) {
        // 箱の作成
        auto enemy = Scene::CreateObjectPtr<Zombie>();
        enemy->SetName("Zombie");

        // vecの方向に向ける
        auto mat = HelperLib::Math::CreateMatrixByFrontVector(front);
        enemy->SetMatrix(mat);

        // posの位置に設定
        enemy->SetTranslate(pos);

        return enemy;
    }

    bool Zombie::Init()    // override
    {
        Super::Init();

        // モデルコンポーネント(0.08倍)
        auto model =
            AddComponent<ComponentModel>("data/Sample/Enemy/model.mv1");

        model->SetScaleAxisXYZ({0.05f});    //

        model->SetAnimation(
            {{"walk", "data/Sample/Enemy/Anim/Walk.mv1", 0, 1.0f},
             {"die", "data/Sample/Enemy/Anim/Death.mv1", 0, 1.0f}});
        model->PlayAnimation("walk", true);

        // コリジョン(カプセル)
        auto col = AddComponent<ComponentCollisionCapsule>();    //
        col->SetTranslate({0, 0, 0});
        col->SetRadius(2.5);
        col->SetHeight(10);
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);
        col->UseGravity();

        //setHP(50);

        return true;
    }

    void Zombie::Update()    // override
    {
        Super::Update();
    }

    // 基本描画の後に処理します
    void Zombie::LateDraw()    // override
    {
        Super::LateDraw();
    }

    void Zombie::GUI()    // override
    {
        Super::GUI();
    }

    void Zombie::OnHit([[maybe_unused]] const ComponentCollision::HitInfo&
                           hitInfo)    // override
    {
        Super::OnHit(hitInfo);
    }

    void Zombie::Idle() {
        Super::Idle();
    }

    // void Zombie::Attack()
    //{
    //     Super::Attack();
    // }

}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Zombie)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Zombie)

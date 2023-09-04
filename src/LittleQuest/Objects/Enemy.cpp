#include "Enemy.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>

#include "Camera.h"

namespace LittleQuest
{

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(Enemy, "LittleQuest/Enemy");

// EnemyPtr Enemy::Create(const float3& pos, const float3& front, const
// std::string name) {
//     // 箱の作成
//     auto enemy = Scene::CreateObjectPtr<Enemy>();
//     enemy->SetName(name);

//    // vecの方向に向ける
//    auto mat = HelperLib::Math::CreateMatrixByFrontVector(front);
//    enemy->SetMatrix(mat);

//    // posの位置に設定
//    enemy->SetTranslate(pos);

//    return enemy;
//}

bool Enemy::Init()   // override
{
    Super::Init();

    setHP();

    return true;
}

void Enemy::Update()   // override
{
    if(isDie) {
        dieTimer--;
        return;
    }

    if(this->HP <= 0) {
        this->Die();
    }
}

// 基本描画の後に処理します
void Enemy::LateDraw()   // override
{
    printfDx("\n%s HP: %i", this->GetName().data(), HP);
}

void Enemy::GUI()   // override
{
    Super::GUI();
}

void Enemy::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)   // override
{
    Super::OnHit(hitInfo);
}

void Enemy::setHP(int HP)
{
    this->HP = HP;
}

void Enemy::Idle()
{
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(HP > 0) {
            modelPtr->PlayAnimation("idle");
        }
    }
}

void Enemy::Attack()
{
}

void Enemy::Damaged(int damage)
{
    this->HP -= damage;
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(HP > 0) {
            modelPtr->PlayAnimation("damaged");
        }
    }
}

void Enemy::Die()
{
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(modelPtr->GetPlayAnimationName() != "die") {
            modelPtr->PlayAnimationNoSame("die");
            RemoveComponent<ComponentCollisionCapsule>();
            this->isDie = true;
        }
    }
}

float Enemy::getDieTimer()
{
    return dieTimer;
}

}   // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Enemy)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Enemy)

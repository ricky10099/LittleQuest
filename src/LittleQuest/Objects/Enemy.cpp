#include "Enemy.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>
//#include <System/SystemMain.h>

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

    state = EnemyState::IDLE;

    //if (!patrolPoint.empty()) {
    //    currPoint = 0;
    //    goal      = patrolPoint[currPoint + 1];
    //    state     = EnemyState::PATROL;
    //}

    animationFrame = 0;

    return true;
}

void Enemy::Update()   // override
{
    // auto curPos = GetTranslate();
    float deltaTime = GetDeltaTime();
    animationFrame++;

    if(isDie) {
        dieTimer--;
        return;
    }

    if(this->HP <= 0) {
        this->Die();
    }

    float3 move;

    switch(state) {
    case EnemyState::DAMAGED:
        CheckDamageAnimation();
        break;
    case EnemyState::ATTACK:
        Attack();
        break;
    case EnemyState::WAIT:
        PatrolWaiting(deltaTime);
        break;
    case EnemyState::PATROL:
        Patrol(move);
        break;
    default:
        Idle();
        break;
    }

    move *= speed_ * GetDeltaTime60();
    // 地面移動スピードを決定する
    AddTranslate(move);
}

// 基本描画の後に処理します
void Enemy::LateDraw()   // override
{
    printfDx("\n%s HP: %i", this->GetName().data(), HP);
    printfDx("\n%s state: %i", this->GetName().data(), state);
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        printfDx("\n%s %s Animation Time:%f",
                 this->GetName().data(),
                 modelPtr->GetPlayAnimationName().data(),
                 modelPtr->GetAnimationTime());
        printfDx("\n%s %s Animation frame:%i",
                 this->GetName().data(),
                 modelPtr->GetPlayAnimationName().data(),
                 animationFrame);
    }
    printfDx("\ngoalx: %f", goal[0]);
    printfDx("\ncurpoint: %i", patrolIndex);
    printfDx("\nnowx: %f", this->GetTranslate().x);
    printfDx("\nx: %f", float3(goal - GetTranslate())[0]);
    printfDx("\nx: %f", float3(goal - GetTranslate())[2]);
}

void Enemy::GUI()   // override
{
    Super::GUI();

    ImGui::DragFloat(u8"goal座標(T)", &goal[0], 0.01f);
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
            modelPtr->PlayAnimationNoSame("idle");
            animationFrame = 0;
        }
    }
}

void Enemy::Patrol(float3& move)
{
    auto pos = GetTranslate();
    pos.y    = 0;
    move     = goal - pos;

    if(abs(move.x) <= float1{1} && abs(move.z) <= float1{1}) {
        patrolIndex++;
        patrolIndex %= patrolPoint.size();
        goal = patrolPoint[patrolIndex];

        PatrolWait(2.f);
        return;
    }

    if(length(move).x > 0) {
        // 動いてる
        move = normalize(move);

        float x     = -move.x;
        float z     = -move.z;
        float theta = atan2(x, z) * RadToDeg - rot_y_;

        // 軸ごと回転 (カメラも一緒に回る)
        SetRotationAxisXYZ({0, theta, 0});
    }
}

void Enemy::PatrolWait(float time)
{
    state = EnemyState::WAIT;
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        modelPtr->PlayAnimation("idle");
    }
    waitTime = time;
}

void Enemy::PatrolWaiting(float deltaTime)
{
    waitTime -= deltaTime;

    if(waitTime <= 0.0f) {
        state = EnemyState::PATROL;
    }
}

void Enemy::Attack()
{
}

void Enemy::CheckDamageAnimation()
{
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(modelPtr->GetPlayAnimationName() == "damaged")
            if(animationFrame >= 40) {
                state = EnemyState::IDLE;
            }
    }
}

void Enemy::Damaged(int damage)
{
    this->HP -= damage;
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(HP > 0) {
            modelPtr->PlayAnimation("damaged");
            animationFrame = 0;
        }
    }
    state = EnemyState::DAMAGED;
}

void Enemy::Die()
{
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(modelPtr->GetPlayAnimationName() != "die") {
            modelPtr->PlayAnimationNoSame("die");
            animationFrame = 0;
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

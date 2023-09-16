#include "Enemy.h"
#include "../Components/ComponentHP.h"

#include <System/Component/Component.h>
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>
// #include <System/SystemMain.h>

#include "Camera.h"

namespace LittleQuest
{

//! @brief Editor上でのCreateObject用の設定
//! @detail BP_OBJECT_TYPEとセットで用意する
BP_OBJECT_IMPL(Enemy, "LittleQuest/Enemy");

bool Enemy::Init()   // override
{
    Super::Init();

    setHP();

    state     = EnemyState::IDLE;
    prevState = state;

    if(!patrolPoint.empty()) {
        patrolIndex = 1;
        goal        = patrolPoint[patrolIndex];
        state       = EnemyState::PATROL;
    }

    animCheck = AnimCheck::IDLE;

    //auto HP = AddComponent<ComponentHP>()->SetHP(200);

    return true;
}

void Enemy::Update()   // override
{
    float deltaTime = GetDeltaTime();

    if(isDie) {
        dieTimer--;
        return;
    }

    if(state != EnemyState::GET_HIT && state != EnemyState::ATTACK && state != EnemyState::WAIT) {
        if(isFoundPlayer()) {
            state = EnemyState::CHASING;
        }
        else {
            state = EnemyState::PATROL;
        }
    }

    float3 move;

    switch(animCheck) {
    case AnimCheck::GETTING_HIT:
        CheckDamageAnimation();
        break;
    case AnimCheck::ATTACKING:
        CheckDamageAnimation();
        break;
    case AnimCheck::IDLE:
        break;
    }

    switch(state) {
    case EnemyState::GET_HIT:
        break;
    case EnemyState::CHASING:
        ChasePlayer(move);
        break;
    case EnemyState::ATTACK:
        Attack(move);
        break;
    case EnemyState::WAIT:
        Waiting(deltaTime);
        break;
    case EnemyState::PATROL:
        Patrol(move);
        break;
    case EnemyState::IDLE:
        Idle();
        break;
    }

    move *= speedBase * speedFactor * GetDeltaTime60();
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
        // printfDx("\n%s %s Animation frame:%i", this->GetName().data(),
        //          modelPtr->GetPlayAnimationName().data(),
        //          animationFrame);
    }
    printfDx("\ngoalx: %f", goal[0]);
    printfDx("\ncurpoint: %i", patrolIndex);
    printfDx("\nnowx: %f", this->GetTranslate()[0]);
    printfDx("\nx: %f", float3(goal - GetTranslate())[0]);
    printfDx("\nz: %f", float3(goal - GetTranslate())[2]);
    printfDx("\nisFound: %i", isFoundPlayer());
    printfDx("\ntargetDegree: %f", degree);
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
            modelPtr->PlayAnimationNoSame("idle", true);
            // animationFrame = 0;
        }
    }
}

bool Enemy::isFoundPlayer()
{
    auto player = Scene::GetObjectPtr<Player>("Player");
    //    float3 front  = this->GetTranslate() + float3{0, 0, 1};
    //
    //    float3 target = player->GetTranslate() - this->GetTranslate();
    //    normalize(target);
    //    float targetDot    = dot(-GetMatrix().axisZ(), target);
    //    float targetRadian = acosf(targetDot);
    //    float targetDegree = (targetRadian * 180.0f / 3.14159265f);
    //    degree             = fabsf(targetDegree);
    //
    //    if (fabsf(targetDegree) < 90.f) {
    //        return true;
    //    } else {
    // a        return false;
    //    }
    float3 distance = player->GetTranslate() - this->GetTranslate();
    distance        = abs(distance);
    if(distance.x < 20 && distance.z < 20) {
        return true;
    }
    else {
        return false;
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

        Wait(2.f);
        return;
    }

    if(length(move).x > 0) {
        // 動いてる
        move = normalize(move);

        float x     = -move.x;
        float z     = -move.z;
        float theta = atan2(x, z) * RadToDeg - rot_y;

        // 軸ごと回転 (カメラも一緒に回る)
        SetRotationAxisXYZ({0, theta, 0});
        speedFactor = walkVal;
    }
}

void Enemy::Wait(float time)
{
    prevState = state;
    state     = EnemyState::WAIT;
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        modelPtr->PlayAnimationNoSame("idle", true, 0.3f);
    }
    waitTime = time;
}

void Enemy::Waiting(float deltaTime)
{
    waitTime -= deltaTime;

    if(waitTime <= 0.0f) {
        state = prevState;
    }
}

void Enemy::ChasePlayer(float3& move)
{
    auto player = Scene::GetObjectPtr<Player>("Player");
    auto pos    = GetTranslate();
    move        = player->GetTranslate() - pos;

    if(abs(move.x) <= float1{5} && abs(move.z) <= float1{5}) {
        prevState = state;
        state     = EnemyState::ATTACK;
        return;
    }

    if(auto modelPtr = GetComponent<ComponentModel>()) {
        modelPtr->PlayAnimationNoSame("run", true);
    }
    if(length(move).x > 0) {
        // 動いてる
        move = normalize(move);

        float x     = -move.x;
        float z     = -move.z;
        float theta = atan2(x, z) * RadToDeg - rot_y;

        // 軸ごと回転 (カメラも一緒に回る)
        SetRotationAxisXYZ({0, theta, 0});
        speedFactor = runVal;
    }
}

void Enemy::Attack(float3& move)
{
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        modelPtr->PlayAnimationNoSame("attack", false, 0.5f);
        animCheck = AnimCheck::ATTACKING;
    }
}

void Enemy::CheckDamageAnimation()
{
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(modelPtr->GetPlayAnimationName() == "getHit") {
            if(modelPtr->GetAnimationTime() > 0.56f) {
                state     = EnemyState::IDLE;
                animCheck = AnimCheck::IDLE;
            }
        }
        else if(modelPtr->GetPlayAnimationName() == "attack") {
            if(modelPtr->GetAnimationTime() > 2.2f) {
                Wait(.5f);
            }
        }
    }
}

void Enemy::GetHit(int damage)
{
    this->HP -= damage;
    auto hpcomponent = GetComponent<ComponentHP>();
    hpcomponent->TakeDamage(damage);
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        if(HP > 0) {
            modelPtr->PlayAnimation("getHit", false, 0.25f);
            prevState = state;
            state     = EnemyState::GET_HIT;
            animCheck = AnimCheck::GETTING_HIT;
        }
        else {
            this->Die();
        }
    }
}

void Enemy::Die()
{
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        modelPtr->PlayAnimationNoSame("die");
        // animationFrame = 0;
        RemoveComponent<ComponentCollisionCapsule>();
        this->isDie = true;
    }
}

float Enemy::getDieTimer()
{
    return dieTimer;
}

}   // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Enemy)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Enemy)

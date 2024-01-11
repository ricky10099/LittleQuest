#include "Enemy.h"
#include "Camera.h"
#include "LittleQuest/Components/ComponentHP.h"
#include "LittleQuest/Tool.h"

#include <System/Component/Component.h>
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>

namespace LittleQuest {

BP_OBJECT_IMPL(Enemy, "LittleQuest/Enemy");

bool Enemy::Init() {
    prevState = state = EnemyState::IDLE;

    if(isPatrol) {
        patrolPoint.push_back(this->GetTranslate() + float3{-50, 0, 0});
        patrolPoint.push_back(this->GetTranslate() + float3{50, 0, 0});
    }

    if(!patrolPoint.empty()) {
        patrolIndex = 1;
        goal        = patrolPoint[patrolIndex];
        state       = EnemyState::PATROL;
    }

    initialState = state;

    pPlayer = Scene::GetObjectPtr<Player>("Player");

    return Super::Init();
}

void Enemy::Update() {
    float deltaTime = GetDeltaTime();

    if(isDead) {
        destroyTimer -= deltaTime;
        return;
    }

    if(state != EnemyState::ATTACK && state != EnemyState::GET_HIT) {
        if(!FindPlayer() && state != EnemyState::GIVE_UP && state != EnemyState::WAIT) {
            if(!patrolPoint.empty()) {
                ChangeState(EnemyState::PATROL);
            } else {
                ChangeState(EnemyState::IDLE);
            }
        }
    }

    float3 move;

    switch(state) {
    case EnemyState::GET_HIT:
        if(!pModel.lock()->IsPlaying()) {
            ChangeState(EnemyState::IDLE);
            //isBusy    = false;
        }
        break;
    case EnemyState::GIVE_UP:
        BackToInitialPosition(move);
        break;
    case EnemyState::CHASING:
        ChasePlayer(move);
        break;
    case EnemyState::ATTACK:
        Attack();
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
    AddTranslate(move);
}

// 基本描画の後に処理します
void Enemy::LateDraw() {
    if(Scene::IsEdit()) {
        printfDx("\n%s state: %i", this->GetName().data(), state);
        if(auto modelPtr = GetComponent<ComponentModel>()) {
            printfDx("\n%s %s Animation Time:%f", this->GetName().data(), modelPtr->GetPlayAnimationName().data(),
                     modelPtr->GetAnimationTime());
        }
        printfDx("\ncurpoint: %i", patrolIndex);
        printfDx("\nx distance: %f", float3(goal - GetTranslate())[0]);
        printfDx("\nz distance: %f", float3(goal - GetTranslate())[2]);
        printfDx("\nf(distance): %f", GetDistance(GetTranslate(), goal));
        printfDx("\nisFound: %i", isFoundPlayer);
        printfDx("\ntargetDegree: %f", GetDegreeToPosition(pPlayer.lock()->GetTranslate()));
        printfDx("\ndie timer: %f", destroyTimer);
    }
    pHP.lock()->DrawHPBar();
}

void Enemy::GUI() {
    Super::GUI();
}

void Enemy::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    Super::OnHit(hitInfo);
}

void Enemy::Idle() {
    // prevState = state;
    // state     = EnemyState::IDLE;
    ChangeState(EnemyState::IDLE);
    if(auto modelPtr = GetComponent<ComponentModel>()) {
        modelPtr->PlayAnimationNoSame("idle", true);
    }
}

bool Enemy::FindPlayer() {
    float distance = GetDistance(pPlayer.lock()->GetTranslate(), this->GetTranslate());
    if(distance < 50 && GetDegreeToPosition(pPlayer.lock()->GetTranslate()) < 50) {
        ChangeState(EnemyState::CHASING);
        return true;
    } else if(prevState == EnemyState::CHASING) {
        ChangeState(EnemyState::GIVE_UP);
    }
    return false;
}

void Enemy::BackToInitialPosition(float3& move) {
    auto pos = this->GetTranslate();
    pos.y    = 0;
    move     = spawnPos - pos;
    if(GetDistance(move) > 0.5f) {
        move = normalize(move);

        float x     = -move.x;
        float z     = -move.z;
        float theta = atan2(x, z) * RadToDeg;

        this->SetRotationAxisXYZ({0, theta, 0});
        speedFactor = runVal;
    } else {
        this->ChangeState(initialState);
    }
}

void Enemy::Patrol(float3& move) {
    auto pos        = GetTranslate();
    move            = goal - pos;
    move.y          = 0;
    float moveValue = GetDistance(move);

    if(moveValue < 5.0f) {
        patrolIndex++;
        patrolIndex %= patrolPoint.size();
        goal = patrolPoint[patrolIndex];
        Wait(2.f);
        return;
    }

    if(moveValue > 1.0f) {
        move = normalize(move);

        float x     = -move.x;
        float z     = -move.z;
        float theta = atan2(x, z) * RadToDeg;

        SetRotationAxisXYZ({0, theta, 0});
        speedFactor = walkVal;
    }
}

void Enemy::Wait(float time) {
    ChangeState(EnemyState::WAIT);
    pModel.lock()->PlayAnimationNoSame("idle", true, 0.3f);
    waitTime = time;
}

void Enemy::Waiting(float deltaTime) {
    waitTime -= deltaTime;

    if(waitTime <= 0.0f) {
        ChangeState(EnemyState::IDLE);
        //animCheck = AnimCheck::NONE;
        //isBusy    = false;
    }
}

void Enemy::ChasePlayer(float3& move) {
    auto pos = GetTranslate();
    move     = pPlayer.lock()->GetTranslate() - pos;

    if(GetDistance(move) < 6.0f) {
        ChangeState(EnemyState::ATTACK);
        //isBusy = true;
        move = {0, 0, 0};
        return;
    }

    pModel.lock()->PlayAnimationNoSame("run", true);

    if(GetDistance(move) > 0) {
        move = normalize(move);

        float x     = -move.x;
        float z     = -move.z;
        float theta = atan2(x, z) * RadToDeg;

        SetRotationAxisXYZ({0, theta, 0});
        speedFactor = runVal;
    }
}

void Enemy::Attack() {
    pModel.lock()->PlayAnimationNoSame("attack", false, 0.5f);
    //animCheck = AnimCheck::ATTACKING;
    if(!pModel.lock()->IsPlaying()) {
        isHitPlayer = false;
        Wait(.5f);
    }
}

//bool Enemy::CheckAnimation() {
//    switch(animCheck) {
//   /* case AnimCheck::GETTING_HIT:
//        if(!pModel.lock()->IsPlaying()) {
//            ChangeState(EnemyState::IDLE);
//            animCheck = AnimCheck::NONE;
//            isBusy    = false;
//            return true;
//        }
//        break;*/
//    case AnimCheck::ATTACKING:
//        if(!pModel.lock()->IsPlaying()) {
//            isAttack    = false;
//            isHitPlayer = false;
//            Wait(.5f);
//            return true;
//        }
//        break;
//    default:
//        return true;
//    }
//
//    return false;
//}

void Enemy::GetHit(int damage) {
    pHP.lock()->TakeDamage(damage);

    if(pHP.lock()->GetHP() > 0) {
        pModel.lock()->PlayAnimation("getHit", false, 0.25f);
        ChangeState(EnemyState::GET_HIT);
        //animCheck = AnimCheck::GETTING_HIT;
        //isBusy    = true;
    } else {
        this->Die();
    }
}

void Enemy::ChangeState(EnemyState fState) {
    prevState   = this->state;
    this->state = fState;
}

void Enemy::Die() {
    pModel.lock()->PlayAnimationNoSame("die");
    RemoveComponent<ComponentCollisionCapsule>();
    this->isDead = true;
}

float Enemy::getDestroyTimer() {
    return destroyTimer;
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Enemy)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Enemy)

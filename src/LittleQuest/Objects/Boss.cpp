#include "Boss.h"
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

BP_OBJECT_IMPL(Boss, "LittleQuest/Boss");

BossPtr Boss::Create(const float3& pos) {
    auto enemy = Scene::CreateObjectDelayInitialize<Boss>();
    auto mat   = HelperLib::Math::CreateMatrixByFrontVector({0, 0, 1});
    enemy->SetName("Boss");
    enemy->SetMatrix(mat);
    enemy->SetTranslate(pos);
    enemy->m_pHP = enemy->AddComponent<ComponentHP>();
    enemy->m_pHP.lock()->SetHP(200);
    enemy->m_spawnPos   = pos;
    enemy->m_spawnPos.y = 0;

    return enemy;
}
bool Boss::Init() {
    m_state   = BossState::IDLE;
    m_pPlayer = Scene::GetObjectPtr<Player>("Player");

    return Super::Init();
}

void Boss::Update() {
    //float deltaTime = GetDeltaTime();

    //if(isDead) {
    //    destroyTimer -= deltaTime;
    //    return;
    //}

    //if(state != BossState::ATTACK && state != BossState::GET_HIT) {
    //    if(!FindPlayer() && state != BossState::GIVE_UP && state != BossState::WAIT) {
    //        if(!patrolPoint.empty()) {
    //            ChangeState(BossState::PATROL);
    //        } else {
    //            ChangeState(BossState::IDLE);
    //        }
    //    }
    //}

    //float3 move;

    //switch(state) {
    //case BossState::GET_HIT:
    //    if(!pModel.lock()->IsPlaying()) {
    //        ChangeState(BossState::IDLE);
    //        //isBusy    = false;
    //    }
    //    break;
    //case BossState::GIVE_UP:
    //    BackToInitialPosition(move);
    //    break;
    //case BossState::CHASING:
    //    ChasePlayer(move);
    //    break;
    //case BossState::ATTACK:
    //    Attack();
    //    break;
    //case BossState::WAIT:
    //    Waiting(deltaTime);
    //    break;
    //case BossState::PATROL:
    //    Patrol(move);
    //    break;
    //case BossState::IDLE:
    //    Idle();
    //    break;
    //}

    //move *= speedBase * speedFactor * GetDeltaTime60();
    //AddTranslate(move);
}

// 基本描画の後に処理します
void Boss::LateDraw() {
    //#if defined _DEBUG
    //    printfDx("\n%s state: %i", this->GetName().data(), state);
    //    if(auto modelPtr = GetComponent<ComponentModel>()) {
    //        printfDx("\n%s %s Animation Time:%f", this->GetName().data(), modelPtr->GetPlayAnimationName().data(),
    //                 modelPtr->GetAnimationTime());
    //    }
    //    printfDx("\ncurpoint: %i", patrolIndex);
    //    printfDx("\nx distance: %f", float3(goal - GetTranslate())[0]);
    //    printfDx("\nz distance: %f", float3(goal - GetTranslate())[2]);
    //    printfDx("\nf(distance): %f", GetDistance(GetTranslate(), goal));
    //    printfDx("\nisFound: %i", isFoundPlayer);
    //    printfDx("\ntargetDegree: %f", GetDegreeToPosition(pPlayer.lock()->GetTranslate()));
    //    printfDx("\ndie timer: %f", destroyTimer);
    //#endif
    //    pHP.lock()->DrawHPBar();
}

void Boss::GUI() {
    Super::GUI();
}

void Boss::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    Super::OnHit(hitInfo);
}

void Boss::Idle() {
    // prevState = state;
    // state     = BossState::IDLE;
    //ChangeState(BossState::IDLE);
    //if(auto modelPtr = GetComponent<ComponentModel>()) {
    //    modelPtr->PlayAnimationNoSame("idle", true);
    //}
}

bool Boss::FindPlayer() {
    //float distance = GetDistance(pPlayer.lock()->GetTranslate(), this->GetTranslate());
    //if(distance < 50 && GetDegreeToPosition(pPlayer.lock()->GetTranslate()) < 50) {
    //    ChangeState(BossState::CHASING);
    //    return true;
    //} else if(prevState == BossState::CHASING) {
    //    ChangeState(BossState::GIVE_UP);
    //}
    //return false;
    return false;
}

void Boss::Wait(float time) {
    //ChangeState(BossState::WAIT);
    //pModel.lock()->PlayAnimationNoSame("idle", true, 0.3f);
    //waitTime = time;
}

void Boss::Waiting(float deltaTime) {
    //waitTime -= deltaTime;

    //if(waitTime <= 0.0f) {
    //    ChangeState(BossState::IDLE);
    //    //animCheck = AnimCheck::NONE;
    //    //isBusy    = false;
    //}
}

void Boss::ChasePlayer(float3& move) {
    //auto pos = GetTranslate();
    //move     = pPlayer.lock()->GetTranslate() - pos;

    //if(GetDistance(move) < 6.0f) {
    //    ChangeState(BossState::ATTACK);
    //    //isBusy = true;
    //    move = {0, 0, 0};
    //    return;
    //}

    //pModel.lock()->PlayAnimationNoSame("run", true);

    //if(GetDistance(move) > 0) {
    //    move = normalize(move);

    //    float x     = -move.x;
    //    float z     = -move.z;
    //    float theta = atan2(x, z) * RadToDeg;

    //    SetRotationAxisXYZ({0, theta, 0});
    //    speedFactor = runVal;
    //}
}

void Boss::Attack() {
    //pModel.lock()->PlayAnimationNoSame("attack", false, 0.5f);
    ////animCheck = AnimCheck::ATTACKING;
    //if(!pModel.lock()->IsPlaying()) {
    //    isHitPlayer = false;
    //    Wait(.5f);
    //}
}

//bool Boss::CheckAnimation() {
//    switch(animCheck) {
//   /* case AnimCheck::GETTING_HIT:
//        if(!pModel.lock()->IsPlaying()) {
//            ChangeState(BossState::IDLE);
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

void Boss::GetHit(int damage) {
    //pHP.lock()->TakeDamage(damage);

    //if(pHP.lock()->GetHP() > 0) {
    //    pModel.lock()->PlayAnimation("getHit", false, 0.25f);
    //    ChangeState(BossState::GET_HIT);
    //    //animCheck = AnimCheck::GETTING_HIT;
    //    //isBusy    = true;
    //} else {
    //    this->Die();
    //}
}

void Boss::ChangeState(BossState state) {
    //prevState   = this->state;
    //this->state = state;
}

void Boss::Die() {
    //pModel.lock()->PlayAnimationNoSame("die");
    //RemoveComponent<ComponentCollisionCapsule>();
    //this->isDead = true;
}
//
//float Boss::getDestroyTimer() {
//    return destroyTimer;
//}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Boss)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Boss)

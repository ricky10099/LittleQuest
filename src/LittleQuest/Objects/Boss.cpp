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
    auto pBoss = Scene::CreateObjectDelayInitialize<Boss>();
    pBoss->SetName("Boss");
    pBoss->SetMatrix(HelperLib::Math::CreateMatrixByFrontVector({0, 0, 1}));
    pBoss->SetTranslate(pos);

    pBoss->m_spawnPos = pos;
    //pBoss->m_spawnPos.y = 0;

    return pBoss;
}
bool Boss::Init() {
    m_pModel = AddComponent<ComponentModel>("data/LittleQuest/Model/MawJLaygo/MawJLaygo.mv1");
    m_pModel.lock()->SetScaleAxisXYZ({0.2f});
    m_pModel.lock()->SetAnimation({
        {              "Idle",        "data/LittleQuest/Anim/MutantSet/MutantIdle.mv1", 0, 1.0f},
        {              "Walk",     "data/LittleQuest/Anim/MutantSet/MutantWalking.mv1", 0, 1.0f},
        {               "Run",         "data/LittleQuest/Anim/MutantSet/MutantRun.mv1", 0, 1.0f},
        { STR(BossAnim::SWIP),     "data/LittleQuest/Anim/MutantSet/MutantSwiping.mv1", 0, 1.0f},
        {STR(BossAnim::PUNCH),       "data/LittleQuest/Anim/MutantSet/MutantPunch.mv1", 0, 1.0f},
        {        "JumpAttack",  "data/LittleQuest/Anim/MutantSet/MutantJumpAttack.mv1", 0, 1.0f},
        {          "Backflip",                    "data/LittleQuest/Anim/Backflip.mv1", 0, 1.0f},
        {        "FlexMuscle", "data/LittleQuest/Anim/MutantSet/MutantFlesMuscles.mv1", 0, 1.0f},
        {              "Roar",     "data/LittleQuest/Anim/MutantSet/MutantRoaring.mv1", 0, 1.0f},
        {             "Taunt",             "data/LittleQuest/Anim/MutantSet/Taunt.mv1", 0, 1.0f},
        {            "GetHit",                   "data/LittleQuest/Anim/HitToBody.mv1", 0, 2.0f},
        {               "Die",       "data/LittleQuest/Anim/MutantSet/MutantDying.mv1", 0, 1.0f}
    });
    m_pModel.lock()->PlayAnimation("Idle", true);
    SetAnimList();

    m_pBody = AddComponent<ComponentCollisionCapsule>();
    m_pBody.lock()->SetTranslate({0, 0, -4});
    m_pBody.lock()->UseGravity();
    m_pBody.lock()->SetHeight(35);
    m_pBody.lock()->SetRadius(6.5);
    m_pBody.lock()->SetMass(100.0f);
    m_pBody.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);

    m_pLeftHand = AddComponent<ComponentCollisionCapsule>();
    m_pLeftHand.lock()->AttachToModel("mixamorig:LeftHand");
    m_pLeftHand.lock()->SetTranslate({0, -25, 0});
    m_pLeftHand.lock()->SetHeight(15.0f);
    m_pLeftHand.lock()->SetRadius(3.0f);
    m_pLeftHand.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
    m_pLeftHand.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);

    m_pRightHand = AddComponent<ComponentCollisionCapsule>();
    m_pRightHand.lock()->AttachToModel("mixamorig:RightHand");
    m_pRightHand.lock()->SetTranslate({0, -25, 0});
    m_pRightHand.lock()->SetHeight(15.0f);
    m_pRightHand.lock()->SetRadius(3.0f);
    m_pRightHand.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
    m_pRightHand.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);

    m_pHP = AddComponent<ComponentHP>();
    m_pHP.lock()->SetHP(1000);

    m_bossCombo = BossCombo::NONE;
    m_state     = BossState::IDLE;
    m_pPlayer   = Scene::GetObjectPtr<Player>("Player");

    return Super::Init();
}

void Boss::Update() {
    float deltaTime = GetDeltaTime();

    if(m_bossCombo == BossCombo::NONE) {
        SelectCombo();
    }

    switch(m_state) {
    case BossState::IDLE:
        m_pModel.lock()->PlayAnimation("Idle");
        break;
    case BossState::ATTACK:
        Attack();
    case BossState::WAIT:
        //Wait();
        break;
    }
    //m_pModel.lock()->GetNodeMatrix("mixamorigs::Hip")._11_12_13;
    //m_pBody.lock()->SetTranslate(m_pModel.lock()->GetNodePosition("mixamorig:Hips"));
    //if (!m_pModel.lock()->IsPlaying()) {
    //SetTranslate(m_pModel.lock()->GetNodePosition("mixamorig:Hips"));
    //}
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
void        Boss::LateDraw() {
#if defined _DEBUG
    printfDx("\ncombo:%i", m_combo);
    printfDx("\nAnim:%i", m_anim);

#endif
    DrawSphere3D(cast(GetTranslate()), 2, 2, GetColor(0, 0, 255), GetColor(255, 0, 0), TRUE);
}

void Boss::GUI() {
    Super::GUI();
}

void Boss::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    Super::OnHit(hitInfo);
}

void Boss::Idle() {}

void Boss::Wait(/*float time*/) {
    //if(m_state != BossState::WAIT) {
    //    ChangeState(BossState::WAIT);
    //    //m_waitTime = time;
    //}
    m_bossCombo = BossCombo::NONE;
    m_combo     = 0;
    m_pModel.lock()->PlayAnimationNoSame("Idle", true, 0.3f);
    m_waitTime -= GetDeltaTime();

    if(m_waitTime <= 0.0f) {
        ChangeState(BossState::IDLE);
    }
}

bool Boss::FindPlayer() {
    return false;
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

void Boss::SelectCombo() {
    if(GetDistance(this->GetTranslate(), m_pPlayer.lock()->GetTranslate()) < 100) {
        ChangeState(BossState::ATTACK);
        m_bossCombo = BossCombo::COMBO5;
        m_combo     = 1;
    }
}

void Boss::Attack() {
    switch(m_bossCombo) {
    case BossCombo::COMBO5:
        Combo5();
        break;
    }
}

void Boss::AttackAnimation(std::string animName, AnimInfo animInfo) {
    if(m_pModel.lock()->GetPlayAnimationName() != animName) {
        //this->SetModelRotation();
        m_pModel.lock()->PlayAnimationNoSame(animName, false, 0.2F, animInfo.animStartTime);
    }
    float currAnimTime = m_pModel.lock()->GetAnimationPlayTime();
    if(currAnimTime > animInfo.triggerStartTime) {
        m_pAttackCol.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);
    }
    if(currAnimTime > animInfo.triggerEndTime) {
        m_pAttackCol.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    }
    if(currAnimTime > animInfo.animCutInTime) {
        m_combo++;
    }
}

void Boss::Combo5() {
    //if(m_bossCombo != BossCombo::COMBO5) {
    //    m_combo     = 1;
    //    m_bossCombo = BossCombo::COMBO5;
    //}
    switch(m_combo) {
    case 1:
    case 5:
        m_pAttackCol = m_pLeftHand.lock();
        AttackAnimation(STR(BossAnim::SWIP), m_animList[STR(BossAnim::SWIP)]);
        break;
    case 3:
        m_pAttackCol = m_pLeftHand.lock();
        AttackAnimation(STR(BossAnim::SWIP), m_animList[STR(BossAnim::QUICK_SWIP)]);
        break;
    case 2:
    case 4:
        m_pAttackCol = m_pRightHand.lock();
        AttackAnimation(STR(BossAnim::PUNCH), m_animList[STR(BossAnim::PUNCH)]);
        break;
    default:
        ChangeState(BossState::WAIT);
        m_waitTime = 1.0f;
        break;
    }
}

void Boss::GetHit(int damage) {
    m_pHP.lock()->TakeDamage(damage);

    if(m_pHP.lock()->GetHP() > 0) {
        //pModel.lock()->PlayAnimation("getHit", false, 0.25f);
        ChangeState(BossState::GET_HIT);
        //animCheck = AnimCheck::GETTING_HIT;
        //isBusy    = true;
    } else {
        this->Die();
    }
}

void Boss::Die() {
    //pModel.lock()->PlayAnimationNoSame("die");
    //RemoveComponent<ComponentCollisionCapsule>();
    //this->isDead = true;
}

void Boss::ChangeState(BossState state) {
    m_prevState   = this->m_state;
    this->m_state = state;
}

void Boss::SetAnimList() {
    AnimInfo info         = {};
    info.triggerStartTime = 75;
    info.triggerEndTime   = 87;
    info.animCutInTime    = 100;

    m_animList[STR(BossAnim::SWIP)] = info;

    info.animStartTime = 75;

    m_animList[STR(BossAnim::QUICK_SWIP)] = info;

    info                  = {};
    info.triggerStartTime = 15;
    info.triggerEndTime   = 29;
    info.animCutInTime    = 35;

    m_animList[STR(BossAnim::PUNCH)] = info;
}
//
//float Boss::getDestroyTimer() {
//    return destroyTimer;
//}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Boss)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Boss)

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
BossPtr Boss::Create(const float3& pos) {
    auto pBoss = Scene::CreateObjectDelayInitialize<Boss>();
    pBoss->SetName("Boss");
    pBoss->SetMatrix(HelperLib::Math::CreateMatrixByFrontVector({0, 0, 1}));
    pBoss->SetTranslate(pos);

    return pBoss;
}

bool Boss::Init() {
    m_pModel = AddComponent<ComponentModel>("data/LittleQuest/Model/MawJLaygo/MawJLaygo.mv1");
    m_pModel.lock()->SetScaleAxisXYZ({0.15f});
    m_pModel.lock()->SetAnimation({
        {       STR(BossState::IDLE),           "data/LittleQuest/Anim/MutantSet/MutantIdle.mv1", 0, 1.0f},
        {                     "Walk",        "data/LittleQuest/Anim/MutantSet/MutantWalking.mv1", 0, 1.0f},
        {  STR(BossState::TURN_LEFT),       "data/LittleQuest/Anim/MutantSet/MutantLeftTurn.mv1", 0, 2.0f},
        { STR(BossState::TURN_RIGHT),      "data/LittleQuest/Anim/MutantSet/MutantRightTurn.mv1", 0, 2.0f},
        { STR(BossAnim::SWIP_ATTACK),        "data/LittleQuest/Anim/MutantSet/MutantSwiping.mv1", 0, 1.0f},
        {       STR(BossAnim::PUNCH),          "data/LittleQuest/Anim/MutantSet/MutantPunch.mv1", 0, 1.0f},
 //{ STR(BossAnim::JUMP_ATTACK),     "data/LittleQuest/Anim/MutantSet/MutantJumpAttack.mv1", 0, 1.0f},
        {    STR(BossAnim::BACKFLIP),                       "data/LittleQuest/Anim/Backflip.mv1", 0, 1.0f},
        {STR(BossAnim::DOUBLE_PUNCH), "data/LittleQuest/Anim/MutantSet/MutantFlexingMuscles.mv1", 0, 1.0f},
        {      STR(BossAnim::CHARGE),            "data/LittleQuest/Anim/MutantSet/Battlecry.mv1", 0, 1.0f},
 //{        STR(BossAnim::ROAR),        "data/LittleQuest/Anim/MutantSet/MutantRoaring.mv1", 0, 1.0f},
        {  STR(BossAnim::TAUNT_ANIM),                "data/LittleQuest/Anim/MutantSet/Taunt.mv1", 0, 1.0f},
        {     STR(BossAnim::EXPLODE),             "data/LittleQuest/Anim/Boss/ChargeExplode.mv1", 0, 1.0f},
        {    STR(BossState::GET_HIT),             "data/LittleQuest/Anim/MutantSet/HeavyHit.mv1", 0, 1.0f},
        {       STR(BossState::DEAD),          "data/LittleQuest/Anim/MutantSet/ZombieDeath.mv1", 0, 1.0f},
    });
    m_pModel.lock()->PlayAnimation(STR(BossState::IDLE), true);

    SetAnimList();
    SetComboList();

    m_powerUpEffect          = LoadEffekseerEffect("data/LittleQuest/Effect/PowerUp.efk", 20.0f);
    m_punchEffect            = LoadEffekseerEffect("data/LittleQuest/Effect/PunchSprite.efk", 2.5f);
    m_powerPunchEffect       = LoadEffekseerEffect("data/LittleQuest/Effect/PunchSprite2.efk", 2.5f);
    m_doublePunchEffect      = LoadEffekseerEffect("data/LittleQuest/Effect/DoublePunchSprite.efk", 1.5f);
    m_powerDoublePunchEffect = LoadEffekseerEffect("data/LittleQuest/Effect/DoublePunchSprite2.efk", 1.5f);
    m_pEffectList            = new int[4]{m_punchEffect, m_powerPunchEffect, m_doublePunchEffect, m_powerDoublePunchEffect};

    m_pBodyBox = AddComponent<ComponentCollisionCapsule>();
    m_pBodyBox.lock()->SetTranslate({0, 0, -4});
    m_pBodyBox.lock()->UseGravity();
    m_pBodyBox.lock()->SetHeight(30);
    m_pBodyBox.lock()->SetRadius(6.5);
    m_pBodyBox.lock()->SetMass(100.0f);
    m_pBodyBox.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);

    m_pLeftHandBox = AddComponent<ComponentCollisionCapsule>();
    m_pLeftHandBox.lock()->AttachToModel("mixamorig:LeftHand");
    m_pLeftHandBox.lock()->SetTranslate({0, -50, 0});
    m_pLeftHandBox.lock()->SetHeight(15.0f);
    m_pLeftHandBox.lock()->SetRadius(7.4f);
    m_pLeftHandBox.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
    m_pLeftHandBox.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    m_pLeftHandBox.lock()->Overlap(~(u32)ComponentCollision::CollisionGroup::NONE);

    m_pRightHandBox = AddComponent<ComponentCollisionCapsule>();
    m_pRightHandBox.lock()->AttachToModel("mixamorig:RightHand");
    m_pRightHandBox.lock()->SetTranslate({0, -50, 0});
    m_pRightHandBox.lock()->SetHeight(15.0f);
    m_pRightHandBox.lock()->SetRadius(7.4f);
    m_pRightHandBox.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
    m_pRightHandBox.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    m_pRightHandBox.lock()->Overlap(~(u32)ComponentCollision::CollisionGroup::NONE);

    m_pHP = AddComponent<ComponentHP>();
    m_pHP.lock()->SetType(ComponentHP::HP_TYPE::BOSS);
    m_pHP.lock()->SetHP(MAX_HP);

    m_bossCombo = BossCombo::NONE;
    m_state     = BossState::IDLE;
    m_pPlayer   = Scene::GetObjectPtr<Player>("Player");

    m_attackSE = LoadSoundMem("data/LittleQuest/Audio/SE/BossAttack.wav");

    srand((unsigned)time(NULL));

    return Super::Init();
}

void Boss::Update() {
#if 0
    return;
#endif    // _DEBUG

    switch(m_sceneState) {
    case Scene::SceneState::TRANS_IN:
        TransInAction();
        break;
    case Scene::SceneState::GAME:
        GameAction();
        break;
    case Scene::SceneState::TRANS_OUT:
        TransOutAction();
        break;
    }
}

void Boss::TransInAction() {
    switch(m_state) {
    case BossState::IDLE:
        m_pModel.lock()->PlayAnimationNoSame(STR(BossState::IDLE));
        break;
    case BossState::TAUNT:
        Taunt();
        break;
    }
}

void Boss::GameAction() {
    if(m_damageTimer > 0) {
        m_damageTimer -= GetDeltaTime60();
    } else {
        m_damageCount = 0;
    }

    if(m_bossCombo == BossCombo::NONE && m_state == BossState::IDLE) {
        if(!m_isAngry) {
            SelectAction();
        } else {
            SelectAngryAction();
        }

        if(m_pHP.lock()->GetHPRate() < 50.0f && !m_isAngry) {
            ChangeState(BossState::ANGRY);
        }
    }
    switch(m_state) {
    case BossState::IDLE:
        Idle();
        break;
    case BossState::ATTACK:
        Attack();
        break;
    case BossState::WAIT:
        Wait();
        break;
    case BossState::TURN_LEFT:
        m_pModel.lock()->PlayAnimationNoSame(STR(BossState::TURN_LEFT));
        if(!m_pModel.lock()->IsPlaying()) {
            ChangeState(BossState::WAIT);
        } else {
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 1);
        }
        break;
    case BossState::TURN_RIGHT:
        m_pModel.lock()->PlayAnimationNoSame(STR(BossState::TURN_RIGHT));
        if(!m_pModel.lock()->IsPlaying()) {
            ChangeState(BossState::WAIT);
        } else {
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 1);
        }
        break;
    case BossState::CHASE:
        ChasePlayer();
        break;
    case BossState::ANGRY:
        PowerUp();
        break;
    case BossState::GET_HIT:
        Damaging();
        break;
    case BossState::DEAD:
        Die();
        break;
    }
}

void Boss::TransOutAction() {
    switch(m_state) {
    case BossState::DEAD:
        Die();
        break;
    default:
        Idle();
        break;
    }
}

void Boss::LateDraw() {
    if(Scene::IsEdit()) {}
    switch(m_sceneState) {
    case Scene::SceneState::TRANS_IN:
        break;
    case Scene::SceneState::GAME:
        if(!m_hideUI) {
            m_pHP.lock()->DrawHPBar();
        }
        break;
    case Scene::SceneState::TRANS_OUT:
        break;
    }

    if(Scene::IsEdit()) {
        printfDx("Boss State: %s\n", bs[(int)m_state]);
    }
}

void Boss::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    if((u32)hitInfo.collision_->GetCollisionGroup() & (u32)ComponentCollision::CollisionGroup::ENEMY_WEAPON) {
        auto* owner = hitInfo.hit_collision_->GetOwner();
        if(auto player = dynamic_cast<Player*>(owner)) {
            if(!m_isHitPlayer) {
                m_isHitPlayer = true;
                player->GetHit((int)(m_comboList[m_bossCombo] * m_attackVal * (1 + m_isAngry)));
            }
        }
    }

    Super::OnHit(hitInfo);
}

void Boss::Idle() {
    m_pModel.lock()->PlayAnimationNoSame(STR(BossState::IDLE), true);
}

void Boss::Wait() {
    m_bossCombo = BossCombo::NONE;
    m_combo     = 0;
    m_pModel.lock()->PlayAnimationNoSame(STR(BossState::IDLE), true, 0.3f);
    m_waitFor -= GetDeltaTime60();

    if(m_waitFor <= 0.0f || (m_pHP.lock()->GetHPRate() < 50.0f && !m_isAngry)) {
        ChangeState(BossState::IDLE);
    }
}

void Boss::ChasePlayer() {
    float3 move     = m_pPlayer.lock()->GetTranslate() - this->GetTranslate();
    float  distance = GetDistance(move);

    m_pModel.lock()->PlayAnimationNoSame("Walk", true);

    if(distance > 0) {
        move = normalize(move);
        SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 10);
        move *= WALK_SPEED * GetDeltaTime60();
        AddTranslate(move);
    }

    if(distance < CLOSE_DISTANCE) {
        ChangeState(BossState::IDLE);
    }
}

void Boss::SelectAction() {
    float  distance  = GetDistance(this->GetTranslate(), m_pPlayer.lock()->GetTranslate());
    float  angle     = GetDegreeToPosition(m_pPlayer.lock()->GetTranslate());
    int    random    = rand() % 100 + 1;
    float3 front     = GetMatrix().axisZ() - GetTranslate();
    float  dotPlayer = VDot(cast(front), cast(m_pPlayer.lock()->GetTranslate()));

    if(angle >= FRONT_ANGLE && angle <= BACK_ANGLE) {
        if(dotPlayer < 0) {
            ChangeState(BossState::TURN_RIGHT);
        } else {
            ChangeState(BossState::TURN_LEFT);
        }
    } else if(distance < TOO_CLOSE_DISTANCE && angle < FRONT_ANGLE) {
        ChangeState(BossState::ATTACK);
        if(random <= 35) {
            m_bossCombo = BossCombo::BACKFLIP_PUNCH;
        } else if(random <= 70) {
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 30);
            m_bossCombo = BossCombo::SWIP;
        } else if(random <= 80) {
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 50);
            m_bossCombo = BossCombo::COMBO5;
        } else {
            if(dotPlayer < 0) {
                ChangeState(BossState::TURN_RIGHT);
            } else {
                ChangeState(BossState::TURN_LEFT);
            }
        }
        m_combo = 1;
    } else if(distance < CLOSE_DISTANCE && angle < FRONT_ANGLE) {
        ChangeState(BossState::ATTACK);
        if(random <= 30) {
            m_bossCombo = BossCombo::CHARGE_EXPLODE;
        } else if(random <= 90) {
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 30);
            m_bossCombo = BossCombo::SWIP;
        } else {
            if(dotPlayer < 0) {
                ChangeState(BossState::TURN_RIGHT);
            } else {
                ChangeState(BossState::TURN_LEFT);
            }
        }
        m_combo = 1;
    } else if(distance < MIDDLE_DISTANCE && angle > BACK_ANGLE) {
        if(random <= 30) {
            ChangeState(BossState::ATTACK);
            m_bossCombo = BossCombo::BACKFLIP_PUNCH;
            m_combo     = 1;
        } else {
            if(dotPlayer < 0) {
                ChangeState(BossState::TURN_RIGHT);
            } else {
                ChangeState(BossState::TURN_LEFT);
            }
        }
    } else if(distance >= CLOSE_DISTANCE && distance <= FAR_DISTANCE) {
        ChangeState(BossState::CHASE);
    } else if(distance > FAR_DISTANCE) {
        ChangeState(BossState::ATTACK);
        if(random <= 70) {
            SetRotationToPosition(m_pPlayer.lock()->GetTranslate());
            m_bossCombo = BossCombo::RANGED_ATTACK;
            m_combo     = 1;
        } else if(random <= 90) {
            SetRotationToPosition(m_pPlayer.lock()->GetTranslate());
            m_bossCombo = BossCombo::CHARGE_PUNCH;
            m_combo     = 1;
        } else {
            ChangeState(BossState::CHASE);
        }
    }
}

void Boss::SelectAngryAction() {
    float  distance  = GetDistance(this->GetTranslate(), m_pPlayer.lock()->GetTranslate());
    float  angle     = GetDegreeToPosition(m_pPlayer.lock()->GetTranslate());
    int    random    = rand() % 100 + 1;
    float3 front     = GetMatrix().axisZ() - GetTranslate();
    float  dotPlayer = VDot(cast(front), cast(m_pPlayer.lock()->GetTranslate()));

    if(angle >= FRONT_ANGLE && angle <= BACK_ANGLE) {
        if(dotPlayer < 0) {
            ChangeState(BossState::TURN_RIGHT);
        } else {
            ChangeState(BossState::TURN_LEFT);
        }
    } else if(distance < TOO_CLOSE_DISTANCE && angle < FRONT_ANGLE) {
        ChangeState(BossState::ATTACK);
        if(random <= 40) {
            m_bossCombo = BossCombo::BACKFLIP_PUNCH;
        } else if(random <= 70) {
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 50);
            m_bossCombo = BossCombo::COMBO5;
        } else if(random <= 90) {
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 30);
            m_bossCombo = BossCombo::SWIP;
        } else {
            if(dotPlayer < 0) {
                ChangeState(BossState::TURN_RIGHT);
            } else {
                ChangeState(BossState::TURN_LEFT);
            }
        }
        m_combo = 1;
    } else if(distance < CLOSE_DISTANCE && angle < FRONT_ANGLE) {
        ChangeState(BossState::ATTACK);
        if(random <= 45) {
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 50);
            m_bossCombo = BossCombo::COMBO5;
        } else if(random <= 75) {
            m_bossCombo = BossCombo::CHARGE_EXPLODE;
        } else {
            if(dotPlayer < 0) {
                ChangeState(BossState::TURN_RIGHT);
            } else {
                ChangeState(BossState::TURN_LEFT);
            }
        }
        m_combo = 1;
    } else if(distance < MIDDLE_DISTANCE && angle > BACK_ANGLE) {
        if(random <= 90) {
            ChangeState(BossState::ATTACK);
            m_bossCombo = BossCombo::BACKFLIP_PUNCH;
            m_combo     = 1;
        } else {
            if(dotPlayer < 0) {
                ChangeState(BossState::TURN_RIGHT);
            } else {
                ChangeState(BossState::TURN_LEFT);
            }
        }
    } else if(distance >= CLOSE_DISTANCE && distance <= FAR_DISTANCE) {
        ChangeState(BossState::CHASE);
    } else if(distance > FAR_DISTANCE) {
        ChangeState(BossState::ATTACK);
        if(random <= 30) {
            SetRotationToPosition(m_pPlayer.lock()->GetTranslate());
            m_bossCombo = BossCombo::RANGED_ATTACK;
            m_combo     = 1;
        } else if(random <= 90) {
            SetRotationToPosition(m_pPlayer.lock()->GetTranslate());
            m_bossCombo = BossCombo::CHARGE_PUNCH;
            m_combo     = 1;
        } else {
            ChangeState(BossState::CHASE);
        }
    }

    if(m_pHP.lock()->GetHPRate() < 15.0f && !m_bigExplode) {
        ChangeState(BossState::ATTACK);
        SetRotationToPosition(m_pPlayer.lock()->GetTranslate());
        m_bossCombo = BossCombo::CHARGE_EXPLODE;
        m_combo     = 1;
    }
}

void Boss::Attack() {
    switch(m_bossCombo) {
    case BossCombo::COMBO5:
        Combo5();
        break;
    case BossCombo::BACKFLIP_PUNCH:
        BackflipPunch();
        break;
    case BossCombo::CHARGE_PUNCH:
        ChargePunch();
        break;
    case BossCombo::SWIP:
        Swip();
        break;
    case BossCombo::RANGED_ATTACK:
        RangedShot();
        break;
    case BossCombo::CHARGE_EXPLODE:
        ChargeExplode();
        break;
    }
}

void Boss::AttackAnimation(std::string animName, AnimInfo& animInfo, std::vector<ComponentCollisionCapsulePtr> atkCol,
                           bool playSE) {
    if(m_pModel.lock()->GetPlayAnimationName() != animName) {
        m_pModel.lock()->PlayAnimationNoSame(animName, false, 0.2F, animInfo.animStartTime);
        m_pModel.lock()->SetAnimationSpeed(animInfo.animStartSpeed);
        m_playedSE = false;
    }
    m_currAnimTime = m_pModel.lock()->GetAnimationPlayTime();
    if(m_currAnimTime >= animInfo.triggerStartTime) {
        if(m_slowMo) {
            m_pModel.lock()->SetAnimationSpeed(animInfo.animSpeed * 0.01f);
        } else {
            m_pModel.lock()->SetAnimationSpeed(animInfo.animSpeed);
        }
        for(int i = 0; i < atkCol.size(); i++) {
            atkCol[i]->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);
        }

        if(!m_playedSE && playSE) {
            PlaySoundMem(m_attackSE, DX_PLAYTYPE_BACK);
            ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), m_attackSE);
            m_playedSE = true;
        }
    }
    if(m_currAnimTime >= animInfo.triggerEndTime) {
        for(int i = 0; i < atkCol.size(); i++) {
            atkCol[i]->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
        }
    }
    if(m_currAnimTime >= animInfo.animCutInTime) {
        m_combo++;
        m_isHitPlayer = false;
    }
}

void Boss::AttackAnimation(std::string animName, AnimInfo& animInfo, bool playSE) {
    AttackAnimation(animName, animInfo, {}, playSE);
}

void Boss::Combo5() {
    switch(m_combo) {
    case 1:
        AttackAnimation(STR(BossAnim::SWIP_ATTACK), m_animList[STR(BossAnim::SWIP_ATTACK)], {m_pLeftHandBox.lock()});
        if(m_currAnimTime >= m_animList[STR(BossAnim::SWIP_ATTACK)].triggerStartTime) {
            m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[0 + m_isAngry]);
            SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, -30 * DegToRad);
        }
        break;
    case 3:
    case 5:
        if(m_isAngry) {
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 1);
        }
        AttackAnimation(STR(BossAnim::SWIP_ATTACK), m_animList[STR(BossAnim::QUICK_SWIP)], {m_pLeftHandBox.lock()});
        if(m_currAnimTime >= m_animList[STR(BossAnim::QUICK_SWIP)].triggerStartTime) {
            m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[0 + m_isAngry]);
            SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, -80 * DegToRad);
        }
        break;
    case 2:
    case 4:
        if(m_isAngry) {
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 1);
        }
        AttackAnimation(STR(BossAnim::PUNCH), m_animList[STR(BossAnim::QUICK_PUNCH)], {m_pRightHandBox.lock()});
        if(m_currAnimTime >= m_animList[STR(BossAnim::QUICK_PUNCH)].triggerStartTime) {
            m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[0 + m_isAngry]);
            SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, 80 * DegToRad);
        }
        break;
    default:
        ChangeState(BossState::WAIT);
        m_waitFor = m_waitTime;
        break;
    }
}

void Boss::BackflipPunch() {
    auto   pos = GetTranslate();
    float3 vec = GetMatrix().axisZ();
    vec.y      = 0;
    switch(m_combo) {
    case 1:
        AttackAnimation(STR(BossAnim::BACKFLIP), m_animList[STR(BossAnim::BACKFLIP)], true);
        vec *= 1.0f * GetDeltaTime60();
        AddTranslate(vec);
        if(m_currAnimTime < m_animList[STR(BossAnim::BACKFLIP)].animCutInTime) {
            m_pBodyBox.lock()->Overlap((u32)ComponentCollision::CollisionGroup::PLAYER);
        } else {
            m_pBodyBox.lock()->Overlap(!(u32)ComponentCollision::CollisionGroup::PLAYER);
        }
        break;
    case 2:
        AttackAnimation(STR(BossAnim::DOUBLE_PUNCH), m_animList[STR(BossAnim::DOUBLE_PUNCH)],
                        {m_pLeftHandBox.lock(), m_pRightHandBox.lock()});
        if(m_currAnimTime < m_animList[STR(BossAnim::DOUBLE_PUNCH)].triggerStartTime) {
            float distance = GetDistance(this->GetTranslate(), m_pPlayer.lock()->GetTranslate());
            vec *= distance * -0.25f * GetDeltaTime60();
            AddTranslate(vec);
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 10);
        } else {
            m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[2 + m_isAngry]);
            SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, 0);
        }
        break;
    default:
        ChangeState(BossState::WAIT);
        m_waitFor = m_waitTime;
        break;
    }
}

void Boss::ChargePunch() {
    auto   pos = GetTranslate();
    float3 vec = GetMatrix().axisZ();
    vec.y      = 0;
    switch(m_combo) {
    case 1:
        AttackAnimation(STR(BossAnim::CHARGE), m_animList[STR(BossAnim::CHARGE)], false);
        break;
    case 2:
        AttackAnimation(STR(BossAnim::DOUBLE_PUNCH), m_animList[STR(BossAnim::DOUBLE_PUNCH)],
                        {m_pLeftHandBox.lock(), m_pRightHandBox.lock()});
        if(m_currAnimTime < m_animList[STR(BossAnim::DOUBLE_PUNCH)].triggerStartTime) {
            float distance = GetDistance(this->GetTranslate(), m_pPlayer.lock()->GetTranslate());
            vec *= distance * -0.25f * GetDeltaTime60();
            AddTranslate(vec);
            SetRotationToPositionWithLimit(m_pPlayer.lock()->GetTranslate(), 10);
        } else {
            m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[2 + m_isAngry]);
            SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, 0);
        }
        break;
    default:
        ChangeState(BossState::WAIT);
        m_waitFor = m_waitTime;
        break;
    }
}

void Boss::Swip() {
    float distance;
    switch(m_combo) {
    case 1:
        AttackAnimation(STR(BossAnim::SWIP_ATTACK), m_animList[STR(BossAnim::SWIP_ATTACK)], {m_pLeftHandBox.lock()});
        if(m_currAnimTime >= m_animList[STR(BossAnim::SWIP_ATTACK)].triggerStartTime) {
            m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[0 + m_isAngry]);
            SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, -30 * DegToRad);
        }
        break;
    case 2:
        distance = GetDistance(this->GetTranslate(), m_pPlayer.lock()->GetTranslate());
        if(distance < 50) {
            AttackAnimation(STR(BossAnim::PUNCH), m_animList[STR(BossAnim::PUNCH)], {m_pRightHandBox.lock()});
            if(m_currAnimTime >= m_animList[STR(BossAnim::PUNCH)].triggerStartTime) {
                m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[0 + m_isAngry]);
                SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                               GetTranslate().y + 12,
                                               GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
                SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, 80 * DegToRad);
            }
        } else {
            m_combo++;
        }
        break;
    default:
        ChangeState(BossState::WAIT);
        m_waitFor = m_waitTime;
        break;
    }
}

void Boss::RangedShot() {
    switch(m_combo) {
    case 1:
        m_combo++;
        break;
    default:
        ChangeState(BossState::WAIT);
        m_waitFor = m_waitTime;
        break;
    }
}

void Boss::ChargeExplode() {
    switch(m_combo) {
    case 1:
        AttackAnimation(STR(BossAnim::EXPLODE_CHARGE), m_animList[STR(BossAnim::EXPLODE_CHARGE)], {m_pLeftHandBox.lock()});
        break;
    default:
        ChangeState(BossState::WAIT);
        m_waitFor = m_waitTime;
        break;
    }
}

void Boss::Taunt() {
    switch(m_combo) {
    case 1:
        AttackAnimation(STR(BossAnim::TAUNT_ANIM), m_animList[STR(BossAnim::TAUNT_ANIM)], false);
        break;
    default:
        ChangeState(BossState::IDLE);
        break;
    }
}

void Boss::PowerUp() {
    m_pModel.lock()->PlayAnimationNoSame(STR(BossAnim::CHARGE));
    m_currAnimTime = m_pModel.lock()->GetAnimationPlayTime();
    if(m_currAnimTime > m_animList[STR(BossAnim::ANGRY_AURA)].triggerStartTime) {
        m_pModel.lock()->SetAnimationSpeed(m_animList[STR(BossAnim::ANGRY_AURA)].animSpeed);
        if(m_pAngryBox.expired()) {
            m_pAngryBox = AddComponent<ComponentCollisionSphere>();
            m_pAngryBox.lock()->SetTranslate({0, 0, 0});
            m_pAngryBox.lock()->SetRadius(20.0f);
            m_pAngryBox.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
            m_pAngryBox.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);
            m_pAngryBox.lock()->Overlap(~(u32)ComponentCollision::CollisionGroup::NONE);
        }

        if(IsEffekseer3DEffectPlaying(m_playingEffect) == -1 && !m_isAngry) {
            m_isAngry       = true;
            m_playingEffect = PlayEffekseer3DEffect(m_powerUpEffect);
            SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x, GetTranslate().y, GetTranslate().z);
        }
    }
    if(m_currAnimTime > m_animList[STR(BossAnim::ANGRY_AURA)].triggerEndTime) {
        if(!m_pAngryBox.expired()) {
            RemoveComponent(m_pAngryBox.lock());
            m_pAngryBox.reset();
            StopEffekseer3DEffect(m_playingEffect);
        }
    }
    if(m_currAnimTime > m_animList[STR(BossAnim::ANGRY_AURA)].animCutInTime) {
        m_combo++;
        m_isHitPlayer = false;
    }

    m_waitTime = ANGRY_WAIT;

    if(!m_pModel.lock()->IsPlaying()) {
        ChangeState(BossState::WAIT);
        m_waitFor = m_waitTime;
    }
}

void Boss::GetHit(int damage) {
    m_pHP.lock()->TakeDamage(damage);
    m_damageCount += damage;

    if(m_pHP.lock()->GetHP() <= 0) {
        ChangeState(BossState::DEAD);
        m_bossCombo = BossCombo::NONE;
        m_combo     = 1;
        return;
    }

    if(m_damageTimer <= 0) {
        m_damageTimer = DAMAGE_TIME;
    }

    if(m_damageCount > DAMAGE_CAP && m_state != BossState::ANGRY) {
        ChangeState(BossState::GET_HIT);
        m_bossCombo   = BossCombo::NONE;
        m_combo       = 1;
        m_damageCount = 0;
    }
}

void Boss::Damaging() {
    switch(m_combo) {
    case 1:
        AttackAnimation(STR(BossState::GET_HIT), m_animList[STR(BossState::GET_HIT)], false);
        break;
    default:
        ChangeState(BossState::WAIT);
        m_waitFor = m_waitTime;
        break;
    }
}

void Boss::Die() {
    m_pModel.lock()->PlayAnimationNoSame(STR(BossState::DEAD));
    if(m_slowMo) {
        m_pModel.lock()->SetAnimationSpeed(0.1f);
    } else {
        m_pModel.lock()->SetAnimationSpeed(1.0f);
    }

    if(m_pBodyBox.lock()) {
        RemoveComponent(m_pBodyBox.lock());
        m_pBodyBox.reset();
    }

    if(!m_pAngryBox.expired()) {
        RemoveComponent(m_pAngryBox.lock());
        m_pAngryBox.reset();
    }
}

void Boss::PlayTaunt() {
    ChangeState(BossState::TAUNT);
    m_combo = 1;
}

void Boss::PlayDead() {
    ChangeState(BossState::DEAD);
}

bool Boss::IsPlayedTaunt() {
    if(m_pModel.lock()->GetOldPlayAnimationName() == STR(BossAnim::TAUNT_ANIM)) {
        ChangeState(BossState::WAIT);
        return true;
    }
    return false;
}

bool Boss::IsDead() {
    return m_pHP.lock()->GetHP() <= 0;
}

void Boss::SlowMo() {
    m_slowMo = true;
    //m_pModel.lock()->SetAnimationSpeed(m_pModel.lock()->GetAnimationSpeed() * 0.01f);
}

void Boss::EndSlowMo() {
    m_slowMo = false;
}

void Boss::ChangeState(BossState state) {
    this->m_state = state;
}

void Boss::SetSceneState(Scene::SceneState state) {
    m_sceneState = state;
    ChangeState(BossState::IDLE);
}

void Boss::Exit() {
    delete[] m_pEffectList;

    DeleteSoundMem(m_attackSE);
    DeleteEffekseerEffect(m_powerUpEffect);
    DeleteEffekseerEffect(m_punchEffect);
    DeleteEffekseerEffect(m_powerPunchEffect);
    DeleteEffekseerEffect(m_doublePunchEffect);
    DeleteEffekseerEffect(m_powerDoublePunchEffect);
}

void Boss::SetAnimList() {
    AnimInfo info         = {};
    info.triggerStartTime = 75;
    info.triggerEndTime   = 83;
    info.animCutInTime    = 83;
    info.animSpeed        = 1.2f;
    info.animStartSpeed   = 2.0f;

    m_animList[STR(BossAnim::SWIP_ATTACK)] = info;

    info.animStartSpeed = 1.0f;
    info.animStartTime  = 75;
    info.animCutInTime  = 83;

    m_animList[STR(BossAnim::QUICK_SWIP)] = info;

    info                  = {};
    info.triggerStartTime = 15;
    info.triggerEndTime   = 29;
    info.animCutInTime    = 35;
    info.animStartSpeed   = 1.0f;
    info.animSpeed        = 1.0f;

    m_animList[STR(BossAnim::PUNCH)] = info;

    info.animStartTime  = 15;
    info.triggerEndTime = 20;
    info.animCutInTime  = 20;

    m_animList[STR(BossAnim::QUICK_PUNCH)] = info;

    info               = {};
    info.animStartTime = 27;
    info.animCutInTime = 90;
    info.animSpeed     = 2.0f;

    m_animList[STR(BossAnim::BACKFLIP)] = info;

    info                  = {};
    info.animStartTime    = 100;
    info.triggerStartTime = 124;
    info.triggerEndTime   = 134;
    info.animCutInTime    = 190;
    info.animSpeed        = 2.0f;
    info.animStartSpeed   = 6.0f;

    m_animList[STR(BossAnim::DOUBLE_PUNCH)] = info;

    info                  = {};
    info.triggerStartTime = 20;
    info.animCutInTime    = 46;
    info.animStartSpeed   = 0.8f;
    info.animSpeed        = 1.0f;

    m_animList[STR(BossAnim::CHARGE)] = info;

    info                  = {};
    info.triggerStartTime = 205;
    info.animStartSpeed   = 0.5f;
    info.animSpeed        = 1.0f;

    m_animList[STR(BossAnim::EXPLODE_CHARGE)] = info;

    //info                  = {};
    //info.animStartTime    = 0;
    //info.triggerStartTime = 102;
    //info.triggerEndTime   = 171;
    //info.animCutInTime    = 172;

    //m_animList[STR(BossAnim::ROAR)] = info;

    info                  = {};
    info.triggerStartTime = 27;
    info.triggerEndTime   = 105;
    info.animCutInTime    = 170;
    info.animStartSpeed   = 0.5f;

    m_animList[STR(BossAnim::ANGRY_AURA)] = info;

    info                = {};
    info.animCutInTime  = 170;
    info.animStartSpeed = 2.0f;
    info.animSpeed      = 2.0f;

    m_animList[STR(BossAnim::TAUNT_ANIM)] = info;

    info               = {};
    info.animCutInTime = 167;

    m_animList[STR(BossState::GET_HIT)] = info;
}

void Boss::SetComboList() {
    m_comboList[BossCombo::SWIP]           = 1;
    m_comboList[BossCombo::COMBO5]         = 0.7f;
    m_comboList[BossCombo::BACKFLIP_PUNCH] = 1;
    m_comboList[BossCombo::CHARGE_PUNCH]   = 2;
    m_comboList[BossCombo::CHARGE_EXPLODE] = 5;
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Boss)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Boss)

#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "LittleQuest/Components/ComponentHP.h"
#include "LittleQuest/Scenes/GameOverScene.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {

BP_OBJECT_IMPL(Player, "LittleQuest/Player");

PlayerPtr Player::Create(const float3& pos, const float3& front) {
    auto player = Scene::CreateObjectPtr<Player>();
    player->SetName("Player");

    auto mat = HelperLib::Math::CreateMatrixByFrontVector(front);
    player->SetMatrix(mat);
    player->SetTranslate(pos);

    player->m_pHP = player->AddComponent<ComponentHP>();
    player->m_pHP.lock()->SetHP(100);

    auto sword = Scene::CreateObjectPtr<Object>("PlayerSword");

    auto model = sword->AddComponent<ComponentModel>();
    model->Load("data/LittleQuest/Model/Sword/Sword.mv1");
    model->SetRotationAxisXYZ({0, 0, 0});
    model->SetScaleAxisXYZ({0.1f, 0.06f, 0.1f});

    auto attach = sword->AddComponent<ComponentAttachModel>();
    attach->SetAttachObject(player, "mixamorig:RightHand");
    attach->SetAttachRotate({0, 0, -90});
    attach->SetAttachOffset({10, 13, -3});

    return player;
}

bool Player::Init() {
    m_pModel = AddComponent<ComponentModel>("data/LittleQuest/Model/Mutant/Mutant.mv1");
    m_pModel.lock()->SetScaleAxisXYZ({0.05f});
    m_pModel.lock()->SetAnimation({
        {   STR(PlayerState::IDLE),           "data/LittleQuest/Anim/AxeSet/AxeIdle.mv1", 0, 1.0f},
        {   STR(PlayerState::JUMP),       "data/LittleQuest/Anim/SwordSet/SwordJump.mv1", 0, 1.0f},
        {   STR(PlayerState::WALK),     "data/LittleQuest/Anim/AxeSet/AxeRunForward.mv1", 0, 1.0f},
        {    STR(PlayerState::RUN),     "data/LittleQuest/Anim/AxeSet/AxeRunForward.mv1", 0, 1.0f},
        {STR(Combo::NORMAL_COMBO1),         "data/LittleQuest/Anim/AxeSet/AxeCombo1.mv1", 0, 3.5f},
        {STR(Combo::NORMAL_COMBO2),         "data/LittleQuest/Anim/AxeSet/AxeCombo2.mv1", 0, 3.5f},
        {STR(Combo::NORMAL_COMBO3), "data/LittleQuest/Anim/AxeSet/AxeAttackDownward.mv1", 0, 2.0f},
        {STR(PlayerState::GET_HIT),   "data/LittleQuest/Anim/SwordSet/Swordm_getHit.mv1", 0, 1.0f}
    });

    SetAnimInfo();

    auto colCap = AddComponent<ComponentCollisionCapsule>();
    colCap->SetTranslate({0, 0, 0});
    colCap->SetRadius(2.5);
    colCap->SetHeight(10);
    colCap->UseGravity();
    colCap->SetCollisionGroup(ComponentCollision::CollisionGroup::PLAYER);

    m_pWeapon = AddComponent<ComponentCollisionCapsule>();
    m_pWeapon.lock()->AttachToModel("mixamorig:RightHand");
    m_pWeapon.lock()->SetTranslate({10, 12, 0});
    m_pWeapon.lock()->SetRotationAxisXYZ({0, 0, -90});
    m_pWeapon.lock()->SetRadius(0.3f);
    m_pWeapon.lock()->SetHeight(5);
    m_pWeapon.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::WEAPON);
    m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    m_pWeapon.lock()->Overlap((u32)ComponentCollision::CollisionGroup::ENEMY);
    m_pWeapon.lock()->SetName("SwordCol");

    m_selfMatrix  = GetMatrix();
    m_getHit      = false;
    m_speedFactor = RUN_SPEED;

    return Super::Init();
}

void Player::Update() {
    m_movement = float3(0, 0, 0);

    if(!m_pCamera.lock()) {
        m_pCamera      = Scene::GetObjectPtr<Camera>("PlayerCamera");
        m_cameraLength = m_pCamera.lock()->GetComponent<ComponentSpringArm>()->GetSpringArmLength();
    } else {
        float3 v     = GetTranslate() - m_pCamera.lock()->GetTranslate();
        m_selfMatrix = HelperLib::Math::CreateMatrixByFrontVector(-v);
    }

    InputHandle();

    if(currCombo != Combo::NO_COMBO) {
        playerState = PlayerState::ATTACK;
    }

    if(IsFloat3Zero(m_movement) && playerState != PlayerState::GET_HIT && playerState != PlayerState::ATTACK) {
        playerState = PlayerState::IDLE;
    }

    switch(playerState) {
    case PlayerState::GET_HIT:
        if(!m_pModel.lock()->IsPlaying()) {
            playerState = PlayerState::IDLE;
        }
        break;
    case PlayerState::ATTACK:
        Attack();
        break;
    case PlayerState::JUMP:
        break;
    case PlayerState::WALK:
        Walk();
        break;
    default:
        Idle();
        break;
    }
}

void Player::LateDraw() {
    m_pHP.lock()->DrawHPBar();
#if defined _DEBUG
    printfDx("\nAnimation Time:%f", m_pModel.lock()->GetAnimationTime());
    printfDx("\nTotal Animation Time:%f", m_pModel.lock()->GetAnimationTotalTime());
    printfDx("\nAnimation Play Time:%f", m_pModel.lock()->GetAnimationPlayTime());
    printfDx("\nANimation Trigger End Time:%f", m_animList[STR(Combo::NORMAL_COMBO1)].triggerEndTime);
    printfDx("\nAnimation Name:%s", m_pModel.lock()->GetPlayAnimationName().data());
    printfDx("\nMovement distance:%f", GetDistance(m_movement));
#endif
}

void Player::GUI() {
    Super::GUI();
}

void Player::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    if(hitInfo.collision_->GetName() == "SwordCol") {
        auto* owner = hitInfo.hit_collision_->GetOwner();

        Enemy* enemy;
        if((enemy = dynamic_cast<Enemy*>(owner)) && currCombo != Combo::NO_COMBO) {
            bool inList = false;
            for(int i = 0; i < m_attackList.size(); i++) {
                if(m_attackList[i] == enemy->GetName().data()) {
                    inList = true;
                    break;
                }
            }

            if(!inList) {
                m_attackList.push_back(enemy->GetName().data());
                enemy->GetHit(this->BASE_ATK);
            }
        }
    }

    Super::OnHit(hitInfo);
}

void Player::GetHit(int damage) {
    m_pHP.lock()->TakeDamage(damage);

    if(m_pHP.lock()->GetHP() > 0) {
        m_pModel.lock()->PlayAnimation(STR(PlayerState::GET_HIT));
        playerState = PlayerState::GET_HIT;
        m_getHit    = true;
    } else {
        Scene::Change(Scene::GetScene<GameOverScene>());
    }
}

void Player::InputHandle() {
    m_cameraLength -= GetMouseWheelRotVol() * 3;
    m_cameraLength = std::min((std::max(m_cameraLength, 10.0f)), 100.0f);
    m_pCamera.lock()->SetCameraLength(m_cameraLength);

    if(IsKeyRepeat(KEY_INPUT_W)) {
        float3 vec = m_selfMatrix.axisZ();
        vec.y      = 0;
        m_movement += -vec;
        playerState = PlayerState::WALK;
    }
    if(IsKeyRepeat(KEY_INPUT_D)) {
        float3 vec = m_selfMatrix.axisX();
        vec.y      = 0;
        m_movement += -vec;
        playerState = PlayerState::WALK;
    }
    if(IsKeyRepeat(KEY_INPUT_S)) {
        float3 vec = m_selfMatrix.axisZ();
        vec.y      = 0;
        m_movement += vec;
        playerState = PlayerState::WALK;
    }
    if(IsKeyRepeat(KEY_INPUT_A)) {
        float3 vec = m_selfMatrix.axisX();
        vec.y      = 0;
        m_movement += vec;
        playerState = PlayerState::WALK;
    }

    if(IsKeyRepeat(KEY_INPUT_LSHIFT)) {
        m_speedFactor = WALK_SPEED;
    } else {
        m_speedFactor = RUN_SPEED;
    }

    if(IsMouseDown(MOUSE_INPUT_LEFT)) {
        playerState = PlayerState::ATTACK;

        if(currCombo == Combo::NO_COMBO) {
            currCombo = Combo::NORMAL_COMBO1;
        } else if(m_waitForCombo) {
            m_isCombo      = true;
            m_waitForCombo = false;
        }
    }
}

void Player::Idle() {
    m_pModel.lock()->PlayAnimationNoSame(STR(PlayerState::IDLE), true, 0.5f);
}

void Player::Walk() {
    if(IsFloat3Zero(m_movement)) {
        return;
    }

    m_movement = normalize(m_movement);
    this->SetModelRotation();
    m_movement *= BASE_SPEED * m_speedFactor * GetDeltaTime60();
    AddTranslate(m_movement);

    m_pModel.lock()->PlayAnimationNoSame(STR(PlayerState::WALK), true, 0.2f, 14.0f);
    m_pModel.lock()->SetAnimationSpeed(GetDistance(m_movement) * 2.0f);
}

void Player::Jump() {}

void Player::Attack() {
    float currAnimTime;
    switch(currCombo) {
    case Combo::NORMAL_COMBO1:
        if(m_pModel.lock()->GetPlayAnimationName() != STR(Combo::NORMAL_COMBO1)) {
            this->SetModelRotation();
            m_pModel.lock()->PlayAnimationNoSame(STR(Combo::NORMAL_COMBO1));
            m_attackList.clear();
        }
        currAnimTime = m_pModel.lock()->GetAnimationPlayTime();
        if(currAnimTime > m_animList[STR(Combo::NORMAL_COMBO1)].triggerStartTime) {
            m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY);
            m_waitForCombo = true;
        }
        if(currAnimTime > m_animList[STR(Combo::NORMAL_COMBO1)].triggerEndTime) {
            m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
        }
        if(currAnimTime > m_animList[STR(Combo::NORMAL_COMBO1)].animCutInTime) {
            currCombo = Combo::NO_COMBO;
            if(m_isCombo) {
                currCombo = Combo::NORMAL_COMBO2;
                m_isCombo = false;
            }
        }
        break;
    case Combo::NORMAL_COMBO2:
        if(m_pModel.lock()->GetPlayAnimationName() != STR(Combo::NORMAL_COMBO2)) {
            m_pModel.lock()->PlayAnimationNoSame(STR(Combo::NORMAL_COMBO2), false, 0.2F,
                                                 m_animList[STR(Combo::NORMAL_COMBO2)].animStartTime);
            this->SetModelRotation();
            m_attackList.clear();
        }
        if(IsKey(KEY_INPUT_SPACE)) {
            m_pModel.lock()->SetAnimationSpeed(1.0f);
        } else {
            m_pModel.lock()->SetAnimationSpeed(3.5f);
        }

        currAnimTime = m_pModel.lock()->GetAnimationPlayTime();
        if(currAnimTime > m_animList[STR(Combo::NORMAL_COMBO2)].triggerStartTime) {
            m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY);
            m_waitForCombo = true;
        }
        if(currAnimTime > m_animList[STR(Combo::NORMAL_COMBO2)].triggerEndTime) {
            m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
        }
        if(currAnimTime > m_animList[STR(Combo::NORMAL_COMBO2)].animCutInTime) {
            currCombo = Combo::NO_COMBO;
            if(m_isCombo) {
                currCombo = Combo::NORMAL_COMBO3;
                m_isCombo = false;
            }
        }
        break;
    case Combo::NORMAL_COMBO3:
        if(m_pModel.lock()->GetPlayAnimationName() != STR(Combo::NORMAL_COMBO3)) {
            this->SetModelRotation();
            m_pModel.lock()->PlayAnimationNoSame(STR(Combo::NORMAL_COMBO3), false, 0.3F,
                                                 m_animList[STR(Combo::NORMAL_COMBO3)].animStartTime);
            m_attackList.clear();
        }
        currAnimTime = m_pModel.lock()->GetAnimationPlayTime();
        if(currAnimTime > m_animList[STR(Combo::NORMAL_COMBO3)].triggerStartTime) {
            m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY);
            m_waitForCombo = true;
        }
        if(currAnimTime > m_animList[STR(Combo::NORMAL_COMBO3)].triggerEndTime) {
            m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
        }
        if(currAnimTime > m_animList[STR(Combo::NORMAL_COMBO3)].animCutInTime) {
            currCombo = Combo::NO_COMBO;
        }
        break;
    default:
        playerState = PlayerState::IDLE;
        currCombo   = Combo::NO_COMBO;
        break;
    }
}

void Player::AttackAnimation(std::string animName, bool isComboFinish, Combo nextCombo) {
    if(m_pModel.lock()->GetPlayAnimationName() != animName) {
        this->SetModelRotation();
        m_pModel.lock()->PlayAnimationNoSame(animName);
        m_attackList.clear();
    }
    float currAnimTime = m_pModel.lock()->GetAnimationPlayTime();
    if(currAnimTime > m_animList[animName].triggerStartTime) {
        m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY);
        m_waitForCombo = true;
    }
    if(currAnimTime > m_animList[animName].triggerEndTime) {
        m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    }
    if(currAnimTime > m_animList[animName].animCutInTime) {
        currCombo = Combo::NO_COMBO;
        if(m_isCombo && !isComboFinish) {
            currCombo = nextCombo;
            m_isCombo = false;
        }
    }
}

void Player::SetModelRotation() {
    if(!IsFloat3Zero(m_movement)) {
        float x     = -m_movement.x;
        float z     = -m_movement.z;
        float theta = atan2(x, z) * RadToDeg;
        m_pModel.lock()->SetRotationAxisXYZ({0, theta, 0});
    }
}

void Player::SetAnimInfo() {
    {
        AnimInfo info         = {};
        info.triggerStartTime = 56;
        info.triggerEndTime   = 66;
        info.animCutInTime    = 82;

        m_animList[STR(Combo::NORMAL_COMBO1)] = info;
    }
    {
        AnimInfo info                         = {};
        info.animStartTime                    = 8;
        info.triggerStartTime                 = 20;
        info.triggerEndTime                   = 35;
        info.animCutInTime                    = 43;
        m_animList[STR(Combo::NORMAL_COMBO2)] = info;
    }
    {
        AnimInfo info                         = {};
        info.animStartTime                    = 33;
        info.triggerStartTime                 = 45;
        info.triggerEndTime                   = 58;
        info.animCutInTime                    = 82;
        m_animList[STR(Combo::NORMAL_COMBO3)] = info;
    }
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Player)

#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "Camera.h"
#include "LittleQuest/Components/ComponentHP.h"
#include "LittleQuest/Components/ComponentCombo.h"
#include "LittleQuest/Scenes/GameOverScene.h"
#include "LittleQuest/Scenes/Stage01.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {

BP_OBJECT_IMPL(Player, "LittleQuest/Player");

PlayerPtr Player::Create(const float3& pos) {
    auto player = Scene::CreateObjectPtr<Player>();

    player->SetName("Player");
    player->SetTranslate(pos);

    return player;
}

bool Player::Init() {
    m_pModel = AddComponent<ComponentModel>("data/LittleQuest/Model/Kachujin/Kachujin.mv1");
    m_pModel.lock()->SetScaleAxisXYZ({0.05f});
    m_pModel.lock()->SetRotationAxisXYZ({0, -90, 0});
    m_pModel.lock()->SetAnimation({
        {    STR(PlayerState::IDLE),           "data/LittleQuest/Anim/AxeSet/AxeIdle.mv1", 0, 1.0f},
        {    STR(PlayerState::ROLL),  "data/LittleQuest/Anim/KachujinSet/ForwardRoll.mv1", 0, 5.0f},
        {    STR(PlayerState::WALK),     "data/LittleQuest/Anim/AxeSet/AxeRunForward.mv1", 0, 1.0f},
        {     STR(PlayerState::RUN),     "data/LittleQuest/Anim/AxeSet/AxeRunForward.mv1", 0, 1.0f},
        { STR(Combo::NORMAL_COMBO1),         "data/LittleQuest/Anim/AxeSet/AxeCombo1.mv1", 0, 1.0f},
        { STR(Combo::NORMAL_COMBO2),         "data/LittleQuest/Anim/AxeSet/AxeCombo2.mv1", 0, 1.0f},
        { STR(Combo::NORMAL_COMBO3), "data/LittleQuest/Anim/AxeSet/AxeAttackDownward.mv1", 0, 1.0f},
        { STR(Combo::NORMAL_COMBO4), "data/LittleQuest/Anim/AxeSet/AxeAttackBackhand.mv1", 0, 1.0f},
        {STR(Combo::SPECIAL_ATTACK),     "data/LittleQuest/Anim/AxeSet/AxeAttack360H.mv1", 0, 1.0f},
        {STR(Combo::SPECIAL_CHARGE),     "data/LittleQuest/Anim/AxeSet/AxeAttack360L.mv1", 0, 1.0f},
        { STR(PlayerState::GET_HIT),    "data/LittleQuest/Anim/KachujinSet/HitToBody.mv1", 0, 3.0f},
        {    STR(PlayerState::DEAD),   "data/LittleQuest/Anim/KachujinSet/SwordDeath.mv1", 0, 1.0f},
    });
    m_pModel.lock()->PlayAnimationNoSame(STR(PlayerState::IDLE), true);

    SetAnimInfo();
    SetComboList();

    {
        m_pSword   = Scene::CreateObjectPtr<Object>("Katana");
        auto model = m_pSword.lock()->AddComponent<ComponentModel>();
        model->Load("data/LittleQuest/Model/Katana/Katana.mv1");
        model->SetRotationAxisXYZ({0, -10, 0});
        model->SetScaleAxisXYZ({3.5f, 4.5f, 4.5f});

        auto attach = m_pSword.lock()->AddComponent<ComponentAttachModel>();
        attach->SetAttachObject(shared_from_this(), "mixamorig:RightHand");
        attach->SetAttachRotate({175, 0, -90});
        attach->SetAttachOffset({-65, 13, -4});
    }
    m_pHP = AddComponent<ComponentHP>();
    m_pHP.lock()->SetType(ComponentHP::HP_TYPE::PLAYER);
    m_pHP.lock()->SetHP(MAX_HP);

    m_pCombo = AddComponent<ComponentCombo>();

    auto colCap = AddComponent<ComponentCollisionCapsule>();
    colCap->SetTranslate({0, 0.5f, 0});
    colCap->SetRadius(1.1f);
    colCap->SetHeight(10.5);
    colCap->UseGravity();
    colCap->SetCollisionGroup(ComponentCollision::CollisionGroup::PLAYER);

    m_pWeapon = AddComponent<ComponentCollisionCapsule>();
    m_pWeapon.lock()->AttachToModel("mixamorig:RightHand");
    m_pWeapon.lock()->SetTranslate({10, 13, -4});
    m_pWeapon.lock()->SetRotationAxisXYZ({0, 0, -92});
    m_pWeapon.lock()->SetRadius(0.2f);
    m_pWeapon.lock()->SetHeight(10.0f);
    m_pWeapon.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::WEAPON);
    m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    m_pWeapon.lock()->Overlap((u32)ComponentCollision::CollisionGroup::ENEMY);

    m_hitEffect      = LoadEffekseerEffect("data/LittleQuest/Effect/LossOfBlood.efk", 0.5f);
    m_slashEffect1   = LoadEffekseerEffect("data/LittleQuest/Effect/SwordSlashSprite1.efk", 5.0f);
    m_slashEffect2   = LoadEffekseerEffect("data/LittleQuest/Effect/SwordSlashSprite2.efk", 5.0f);
    m_slashEffect3   = LoadEffekseerEffect("data/LittleQuest/Effect/SwordSlashSprite3.efk", 5.0f);
    m_pEffectList    = new int[3]{m_slashEffect1, m_slashEffect2, m_slashEffect3};
    m_chargingEffect = LoadEffekseerEffect("data/LittleQuest/Effect/Charging.efk", 2.5f);
    m_chargedEffect  = LoadEffekseerEffect("data/LittleQuest/Effect/Charged.efk", 2.5f);
    m_pChargeList    = new int[2]{m_chargingEffect, m_chargedEffect};

    m_selfMatrix  = GetMatrix();
    m_speedFactor = RUN_SPEED;

    m_swordSE    = LoadSoundMem("data/LittleQuest/Audio/SE/sword-swipes-2-quick.mp3");
    m_swordHitSE = LoadSoundMem("data/LittleQuest/Audio/SE/SwordHit.wav");

    return Super::Init();
}

void Player::Update() {
    switch(m_sceneState) {
    case Scene::SceneState::TRANS_IN:
        break;
    case Scene::SceneState::GAME:
        GameAction();
        break;
    case Scene::SceneState::TRANS_OUT:
        TransOutAction();
        break;
    }
}

void Player::GameAction() {
    m_movement = float3(0, 0, 0);
    m_hitTimer -= GetDeltaTime60();
    m_hitTimer = std::max(0.0f, m_hitTimer);

    if(m_hitTimer <= 0 && m_isHit) {
        m_isHit = false;
    }

    if(!m_pCamera.lock()) {
        m_pCamera      = Scene::GetObjectPtr<Camera>("PlayerCamera");
        m_cameraLength = m_pCamera.lock()->GetComponent<ComponentSpringArm>()->GetSpringArmLength();
    } else {
        float3 v     = m_pCamera.lock()->CameraForward();
        m_selfMatrix = HelperLib::Math::CreateMatrixByFrontVector(v);
    }

    InputHandle();

    if(m_currCombo != Combo::NO_COMBO && m_playerState != PlayerState::ROLL) {
        m_playerState = PlayerState::ATTACK;
    }

    if(IsFloat3Zero(m_movement) && m_playerState != PlayerState::GET_HIT && m_playerState != PlayerState::ATTACK &&
       m_playerState != PlayerState::ROLL && m_playerState != PlayerState::DEAD) {
        m_playerState = PlayerState::IDLE;
    }

    switch(m_playerState) {
    case PlayerState::GET_HIT:
        if(!m_pModel.lock()->IsPlaying()) {
            m_playerState = PlayerState::IDLE;
        }
        break;
    case PlayerState::ATTACK:
        Attack();
        break;
    case PlayerState::ROLL:
        m_pModel.lock()->PlayAnimationNoSame(STR(PlayerState::ROLL));
        m_currCombo = Combo::NO_COMBO;
        m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
        if(m_pModel.lock()->IsPlaying()) {
            m_movement     = {};
            m_isInvincible = true;
            float3 vec     = m_pModel.lock()->GetMatrix().axisZ();
            vec.y          = 0;
            m_movement += -vec;
            m_movement *= 15.0f * GetDeltaTime60();
            AddTranslate(m_movement);
        } else {
            m_isInvincible = false;
            m_playerState  = PlayerState::IDLE;
        }
        break;
    case PlayerState::WALK:
        Walk();
        break;
    case PlayerState::DEAD:
        Die();
        break;
    default:
        Idle();
        break;
    }
}

void Player::TransOutAction() {
    m_pModel.lock()->SetRotationAxisXYZ({0, -90, 0});
    switch(m_playerState) {
    case PlayerState::DEAD:
        Die();
        break;
    default:
        Idle();
        break;
    }
}

void Player::LateDraw() {
    switch(m_sceneState) {
    case Scene::SceneState::TRANS_IN:
        break;
    case Scene::SceneState::GAME:
        m_pHP.lock()->DrawHPBar();
        m_pCombo.lock()->DrawComboBar();
        break;
    case Scene::SceneState::TRANS_OUT:
        break;
    }
}

void Player::GUI() {
    Super::GUI();
}

void Player::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    if((u32)hitInfo.collision_->GetCollisionGroup() & (u32)ComponentCollision::CollisionGroup::WEAPON) {
        auto* owner = hitInfo.hit_collision_->GetOwner();

        Boss* boss;
        if((boss = dynamic_cast<Boss*>(owner)) && m_currCombo != Combo::NO_COMBO) {
            bool inList = false;
            for(int i = 0; i < m_attackList.size(); i++) {
                if(m_attackList[i] == boss->GetName().data()) {
                    inList = true;
                    break;
                }
            }

            if(!inList) {
                m_attackList.push_back(boss->GetName().data());
                boss->GetHit((int)(this->BASE_ATK * m_comboList[m_currCombo] * m_pCombo.lock()->ComboBuff()));
                m_isHit = true;
                m_hitTimer += HIT_PAUSE;
                m_pCombo.lock()->AddCombo(m_comboList[m_currCombo]);
                m_playingEffect = PlayEffekseer3DEffect(m_hitEffect);
                PlaySoundMem(m_swordHitSE, DX_PLAYTYPE_BACK);
                ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), m_swordHitSE);
                SetPosPlayingEffekseer3DEffect(m_playingEffect, hitInfo.hit_position_.x, hitInfo.hit_position_.y,
                                               hitInfo.hit_position_.z);
            }
        }
    }

    Super::OnHit(hitInfo);
}

void Player::GetHit(int damage) {
    if(m_isInvincible) {
        return;
    }

    m_pHP.lock()->TakeDamage(damage);
    StartJoypadVibration(DX_INPUT_PAD1, 250, 500, -1);

    if(m_pHP.lock()->GetHP() <= 0) {
        m_playerState = PlayerState::DEAD;
        return;
    }

    m_pModel.lock()->PlayAnimation(STR(PlayerState::GET_HIT));
    m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    m_playerState = PlayerState::GET_HIT;
    m_currCombo   = Combo::NO_COMBO;
    m_chargeTime  = 0.0f;
}

void Player::InputHandle() {
    DINPUT_JOYSTATE DInputState;
    m_cameraLength -= GetMouseWheelRotVol() * 3;
    if(IsPadRepeat(PAD_ID::PAD_5, DX_PADTYPE_DUAL_SENSE)) {
        ++m_cameraLength;
    }
    if(IsPadRepeat(PAD_ID::PAD_6, DX_PADTYPE_DUAL_SENSE)) {
        --m_cameraLength;
    }
    m_cameraLength = std::min((std::max(m_cameraLength, 10.0f)), 100.0f);
    m_pCamera.lock()->SetCameraLength(m_cameraLength);
    if(m_playerState != PlayerState::ROLL && m_playerState != PlayerState::GET_HIT && m_playerState != PlayerState::DEAD) {
        if(IsKeyRepeat(KEY_INPUT_LSHIFT)) {
            m_speedFactor = WALK_SPEED;
        } else {
            m_speedFactor = RUN_SPEED;
        }

        GetJoypadDirectInputState(DX_INPUT_PAD1, &DInputState);
        switch(GetJoypadType(DX_INPUT_PAD1)) {
        case DX_PADTYPE_DUAL_SENSE:
            m_movement += float3{-DInputState.X, 0, DInputState.Y};
            m_movement = mul(float4{m_movement, 1.0f}, m_selfMatrix).xyz;
            break;
        default:
            break;
        }

        if(IsKeyRepeat(KEY_INPUT_W)) {
            float3 vec = m_selfMatrix.axisZ();
            vec.y      = 0;
            m_movement += -vec;
        }
        if(IsKeyRepeat(KEY_INPUT_D)) {
            float3 vec = m_selfMatrix.axisX();
            vec.y      = 0;
            m_movement += -vec;
        }
        if(IsKeyRepeat(KEY_INPUT_S)) {
            float3 vec = m_selfMatrix.axisZ();
            vec.y      = 0;
            m_movement += vec;
        }
        if(IsKeyRepeat(KEY_INPUT_A)) {
            float3 vec = m_selfMatrix.axisX();
            vec.y      = 0;
            m_movement += vec;
        }

        if(IsMouseDown(MOUSE_INPUT_LEFT) || IsPadDown(PAD_ID::PAD_4, DX_PADTYPE_DUAL_SENSE)) {
            m_playerState = PlayerState::ATTACK;

            if(m_currCombo == Combo::NO_COMBO) {
                m_currCombo = Combo::NORMAL_COMBO1;
            } else if(m_currCombo != Combo::SPECIAL_ATTACK) {
                m_isCombo = true;
            }
        }

        if(IsMouseRepeat(MOUSE_INPUT_RIGHT, 1) || IsPadRepeat(PAD_ID::PAD_3, DX_PADTYPE_DUAL_SENSE)) {
            m_chargeTime += GetDeltaTime60();
            if(m_chargeTime >= SPECIAL_CHARGE_TIME && !m_charged) {
                StopEffekseer3DEffect(m_playingChargeEffect);
                m_charged = true;
            }
            if(IsEffekseer3DEffectPlaying(m_playingChargeEffect) == -1) {
                m_playingChargeEffect = PlayEffekseer3DEffect(m_pChargeList[(m_chargeTime >= SPECIAL_CHARGE_TIME)]);
            }
            SetPosPlayingEffekseer3DEffect(m_playingChargeEffect, GetTranslate().x, GetTranslate().y + 7, GetTranslate().z);
            m_speedFactor = WALK_SPEED;
        } else if(m_chargeTime > 0 && m_currCombo == Combo::NO_COMBO) {
            StopEffekseer3DEffect(m_playingChargeEffect);
            if(m_chargeTime >= SPECIAL_CHARGE_TIME) {
                m_isCombo = true;
            }
            m_charged     = false;
            m_playerState = PlayerState::ATTACK;
            m_currCombo   = Combo::SPECIAL_ATTACK;
            m_chargeTime  = 0;
            m_speedFactor = RUN_SPEED;
        }

        if(!IsFloat3Zero(m_movement)) {
            m_playerState = PlayerState::WALK;
        }
        if(IsKeyDown(KEY_INPUT_SPACE) || IsPadDown(PAD_ID::PAD_2, DX_PADTYPE_DUAL_SENSE)) {
            m_chargeTime = 0;
            m_charged    = false;
            StopEffekseer3DEffect(m_playingChargeEffect);
            m_playerState = PlayerState::ROLL;
            this->SetModelRotation();
        }
    }
}

void Player::Idle() {
    m_pModel.lock()->PlayAnimationNoSame(STR(PlayerState::IDLE), true, 0.5f);
}

void Player::Walk() {
    m_movement = normalize(m_movement);
    this->SetModelRotation();
    m_movement *= BASE_SPEED * m_speedFactor * GetDeltaTime();
    AddTranslate(m_movement);

    m_pModel.lock()->PlayAnimationNoSame(STR(PlayerState::WALK), true, 0.2f, 14.0f);
    m_pModel.lock()->SetAnimationSpeed(GetDistance(m_movement) * 5.0f);
}

void Player::Jump() {}

void Player::Attack() {
    switch(m_currCombo) {
    case Combo::NORMAL_COMBO1:
        AttackAnimation(STR(Combo::NORMAL_COMBO1), m_animList[STR(Combo::NORMAL_COMBO1)], Combo::NORMAL_COMBO2);
        if(m_currAnimTime > m_animList[STR(Combo::NORMAL_COMBO1)].triggerStartTime) {
            if(!m_playedFX) {
                m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[(int)m_pCombo.lock()->ComboBuff() - 1]);
                SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x, GetTranslate().y + 6, GetTranslate().z);
                SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, (m_pModel.lock()->GetRotationAxisXYZ().y) * DegToRad,
                                                    0);

                PlaySoundMem(m_swordSE, DX_PLAYTYPE_BACK);
                ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), m_swordSE);
                m_playedFX = true;
            }
        }
        break;
    case Combo::NORMAL_COMBO2:
        AttackAnimation(STR(Combo::NORMAL_COMBO2), m_animList[STR(Combo::NORMAL_COMBO2)], Combo::NORMAL_COMBO3);
        if(m_currAnimTime > m_animList[STR(Combo::NORMAL_COMBO2)].triggerStartTime) {
            if(!m_playedFX) {
                m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[(int)m_pCombo.lock()->ComboBuff() - 1]);
                SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x, GetTranslate().y + 6, GetTranslate().z);
                SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, (m_pModel.lock()->GetRotationAxisXYZ().y) * DegToRad,
                                                    180 * DegToRad);
                PlaySoundMem(m_swordSE, DX_PLAYTYPE_BACK);
                ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), m_swordSE);
                m_playedFX = true;
            }
        }
        break;
    case Combo::NORMAL_COMBO3:
        AttackAnimation(STR(Combo::NORMAL_COMBO3), m_animList[STR(Combo::NORMAL_COMBO3)], Combo::NORMAL_COMBO4);
        if(m_currAnimTime > m_animList[STR(Combo::NORMAL_COMBO3)].triggerStartTime) {
            if(!m_playedFX) {
                m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[(int)m_pCombo.lock()->ComboBuff() - 1]);
                SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x, GetTranslate().y + 6, GetTranslate().z);
                SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, (m_pModel.lock()->GetRotationAxisXYZ().y) * DegToRad,
                                                    -50 * DegToRad);
                PlaySoundMem(m_swordSE, DX_PLAYTYPE_BACK);
                ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), m_swordSE);
                m_playedFX = true;
            }
        }
        break;
    case Combo::NORMAL_COMBO4:
        AttackAnimation(STR(Combo::NORMAL_COMBO4), m_animList[STR(Combo::NORMAL_COMBO4)]);
        if(m_currAnimTime > m_animList[STR(Combo::NORMAL_COMBO4)].triggerStartTime) {
            if(!m_playedFX) {
                m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[(int)m_pCombo.lock()->ComboBuff() - 1]);
                SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x, GetTranslate().y + 6, GetTranslate().z);
                SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, (m_pModel.lock()->GetRotationAxisXYZ().y) * DegToRad,
                                                    52 * DegToRad);
                PlaySoundMem(m_swordSE, DX_PLAYTYPE_BACK);
                ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), m_swordSE);
                m_playedFX = true;
            }
        }
        break;
    case Combo::SPECIAL_ATTACK:
        if(m_isCombo) {
            AttackAnimation(STR(Combo::SPECIAL_ATTACK), m_animList[STR(Combo::SPECIAL_ATTACK)], Combo::SPECIAL_CHARGE);
        } else {
            AttackAnimation(STR(Combo::SPECIAL_ATTACK), m_animList[STR(Combo::SPECIAL_ATTACK)]);
        }
        if(m_currAnimTime > m_animList[STR(Combo::SPECIAL_ATTACK)].triggerStartTime) {
            if(!m_playedFX) {
                m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[(int)m_pCombo.lock()->ComboBuff() - 1]);
                SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x, GetTranslate().y + 6, GetTranslate().z);
                SetRotationPlayingEffekseer3DEffect(m_playingEffect, 0, (m_pModel.lock()->GetRotationAxisXYZ().y) * DegToRad,
                                                    180 * DegToRad);
                PlaySoundMem(m_swordSE, DX_PLAYTYPE_BACK);
                ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), m_swordSE);
                m_playedFX = true;
            }
        }
        break;
    case Combo::SPECIAL_CHARGE:
        AttackAnimation(STR(Combo::SPECIAL_CHARGE), m_animList[STR(Combo::SPECIAL_CHARGE)]);
        if(m_currAnimTime > m_animList[STR(Combo::SPECIAL_CHARGE)].triggerStartTime) {
            if(!m_playedFX) {
                m_playingEffect = PlayEffekseer3DEffect(m_pEffectList[(int)m_pCombo.lock()->ComboBuff() - 1]);
                SetPosPlayingEffekseer3DEffect(m_playingEffect, GetTranslate().x + 3.5f, GetTranslate().y + 6,
                                               GetTranslate().z);
                SetRotationPlayingEffekseer3DEffect(m_playingEffect, -20 * DegToRad,
                                                    (m_pModel.lock()->GetRotationAxisXYZ().y) * DegToRad, 180 * DegToRad);
                PlaySoundMem(m_swordSE, DX_PLAYTYPE_BACK);
                ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), m_swordSE);
                m_playedFX = true;
            }
        }
        break;
    default:
        m_playerState = PlayerState::IDLE;
        m_currCombo   = Combo::NO_COMBO;
        break;
    }
}

void Player::AttackAnimation(std::string animName, AnimInfo animInfo, Combo nextCombo) {
    if(m_pModel.lock()->GetPlayAnimationName() != animName) {
        this->SetModelRotation();
        m_pModel.lock()->PlayAnimationNoSame(animName, false, 0.2F, m_animList[animName].animStartTime);
        m_pModel.lock()->SetAnimationSpeed(animInfo.animStartSpeed);
        m_attackList.clear();
        m_playedFX = false;
    }
    m_currAnimTime = m_pModel.lock()->GetAnimationPlayTime();
    if(m_currAnimTime > m_animList[animName].triggerStartTime) {
        if(m_isHit) {
            m_pModel.lock()->SetAnimationSpeed(animInfo.animSpeed * 0.05f);
        } else {
            m_pModel.lock()->SetAnimationSpeed(animInfo.animSpeed);
        }
        m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY);
    }
    if(m_currAnimTime > m_animList[animName].triggerEndTime) {
        m_pWeapon.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    }
    if(m_currAnimTime > m_animList[animName].animCutInTime) {
        m_currCombo = Combo::NO_COMBO;
        if(m_isCombo) {
            m_currCombo = nextCombo;
            m_isCombo   = false;
        }
    }
}

void Player::Die() {
    m_pModel.lock()->PlayAnimationNoSame(STR(PlayerState::DEAD));
}

bool Player::IsDead() {
    return m_pHP.lock()->GetHP() <= 0;
}

void Player::PlayDead() {
    m_playerState = PlayerState::DEAD;
}

void Player::SetModelRotation() {
    if(IsFloat3Zero(m_movement)) {
        return;
    }

    float x     = -m_movement.x;
    float z     = -m_movement.z;
    float theta = atan2(x, z) * RadToDeg;
    m_pModel.lock()->SetRotationAxisXYZ({0, theta, 0});
}

void Player::SetSceneState(Scene::SceneState state) {
    m_sceneState  = state;
    m_playerState = PlayerState::IDLE;
}

void Player::SetAnimInfo() {
    AnimInfo info         = {};
    info.animStartTime    = 0;
    info.triggerStartTime = 56;
    info.triggerEndTime   = 66;
    info.animCutInTime    = 82;
    info.animStartSpeed   = 3.5f;
    info.animSpeed        = 3.5f;

    m_animList[STR(Combo::NORMAL_COMBO1)] = info;

    info                  = {};
    info.animStartTime    = 8;
    info.triggerStartTime = 20;
    info.triggerEndTime   = 35;
    info.animCutInTime    = 43;
    info.animStartSpeed   = 3.5f;
    info.animSpeed        = 3.5f;

    m_animList[STR(Combo::NORMAL_COMBO2)] = info;

    info                  = {};
    info.animStartTime    = 33;
    info.triggerStartTime = 45;
    info.triggerEndTime   = 58;
    info.animCutInTime    = 82;
    info.animStartSpeed   = 3.0f;
    info.animSpeed        = 3.0f;

    m_animList[STR(Combo::NORMAL_COMBO3)] = info;

    info                  = {};
    info.animStartTime    = 0;
    info.triggerStartTime = 55;
    info.triggerEndTime   = 68;
    info.animCutInTime    = 88;
    info.animStartSpeed   = 3.0f;
    info.animSpeed        = 3.0f;

    m_animList[STR(Combo::NORMAL_COMBO4)] = info;

    info                  = {};
    info.triggerStartTime = 55;
    info.triggerEndTime   = 72;
    info.animCutInTime    = 75;
    info.animStartSpeed   = 3.0f;
    info.animSpeed        = 3.0f;

    m_animList[STR(Combo::SPECIAL_ATTACK)] = info;

    info                  = {};
    info.animStartTime    = 12;
    info.triggerStartTime = 55;
    info.triggerEndTime   = 65;
    info.animCutInTime    = 80;
    info.animStartSpeed   = 3.0f;
    info.animSpeed        = 3.0f;

    m_animList[STR(Combo::SPECIAL_CHARGE)] = info;
}

void Player::SetComboList() {
    m_comboList[Combo::NORMAL_COMBO1]  = 1;
    m_comboList[Combo::NORMAL_COMBO2]  = 1;
    m_comboList[Combo::NORMAL_COMBO3]  = 2;
    m_comboList[Combo::NORMAL_COMBO4]  = 4;
    m_comboList[Combo::SPECIAL_ATTACK] = 1;
    m_comboList[Combo::SPECIAL_CHARGE] = 6;
}

void Player::Exit() {
    delete[] m_pEffectList;
    delete[] m_pChargeList;

    DeleteSoundMem(m_swordSE);
    DeleteSoundMem(m_swordHitSE);

    DeleteEffekseerEffect(m_slashEffect1);
    DeleteEffekseerEffect(m_slashEffect2);
    DeleteEffekseerEffect(m_slashEffect3);
    DeleteEffekseerEffect(m_hitEffect);

    m_animList.clear();
    m_comboList.clear();
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Player)

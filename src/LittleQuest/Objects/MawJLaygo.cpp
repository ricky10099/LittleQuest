#include "MawJLaygo.h"
#include "Camera.h"
#include "LittleQuest/Components/ComponentHP.h"
#include "LittleQuest/Tool.h"
#include "player.h"

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
MawJLaygoPtr MawJLaygo::Create(const float3& pos) {
    auto pMawJLaygo = Scene::CreateObjectDelayInitialize<MawJLaygo>();
    pMawJLaygo->SetName("MawJLaygo");
    pMawJLaygo->SetMatrix(HelperLib::Math::CreateMatrixByFrontVector({0, 0, 1}));
    pMawJLaygo->SetTranslate(pos);

    return pMawJLaygo;
}

bool MawJLaygo::Init() {
    _model = AddComponent<ComponentModel>("data/LittleQuest/Model/MawJLaygo/MawJLaygo.mv1");
    _model.lock()->SetScaleAxisXYZ({0.15f});
    _model.lock()->SetAnimation({
        {       STR(MawJLaygoState::IDLE),           "data/LittleQuest/Anim/MutantSet/MutantIdle.mv1", 0, 1.0f},
        {                          "Walk",        "data/LittleQuest/Anim/MutantSet/MutantWalking.mv1", 0, 1.0f},
        {  STR(MawJLaygoState::TURN_LEFT),       "data/LittleQuest/Anim/MutantSet/MutantLeftTurn.mv1", 0, 2.0f},
        { STR(MawJLaygoState::TURN_RIGHT),      "data/LittleQuest/Anim/MutantSet/MutantRightTurn.mv1", 0, 2.0f},
        { STR(MawJLaygoAnim::SWIP_ATTACK),        "data/LittleQuest/Anim/MutantSet/MutantSwiping.mv1", 0, 1.0f},
        {       STR(MawJLaygoAnim::PUNCH),          "data/LittleQuest/Anim/MutantSet/MutantPunch.mv1", 0, 1.0f},
        {    STR(MawJLaygoAnim::BACKFLIP),             "data/LittleQuest/Anim/MawJLaygo/Backflip.mv1", 0, 1.0f},
        {STR(MawJLaygoAnim::DOUBLE_PUNCH), "data/LittleQuest/Anim/MutantSet/MutantFlexingMuscles.mv1", 0, 1.0f},
        {      STR(MawJLaygoAnim::CHARGE),            "data/LittleQuest/Anim/MutantSet/Battlecry.mv1", 0, 1.0f},
        {  STR(MawJLaygoAnim::TAUNT_ANIM),                "data/LittleQuest/Anim/MutantSet/Taunt.mv1", 0, 1.0f},
        {     STR(MawJLaygoAnim::EXPLODE),        "data/LittleQuest/Anim/MawJLaygo/ChargeExplode.mv1", 0, 1.0f},
        {    STR(MawJLaygoState::GET_HIT),             "data/LittleQuest/Anim/MutantSet/HeavyHit.mv1", 0, 1.0f},
        {       STR(MawJLaygoState::DEAD),          "data/LittleQuest/Anim/MutantSet/ZombieDeath.mv1", 0, 1.0f},
    });
    _model.lock()->PlayAnimation(STR(MawJLaygoState::IDLE), true);

    SetAnimList();
    SetComboList();

    _powerUpEffect          = LoadEffekseerEffect("data/LittleQuest/Effect/PowerUp.efk", 20.0f);
    _punchEffect            = LoadEffekseerEffect("data/LittleQuest/Effect/PunchSprite.efk", 2.5f);
    _powerPunchEffect       = LoadEffekseerEffect("data/LittleQuest/Effect/PunchSprite2.efk", 2.5f);
    _doublePunchEffect      = LoadEffekseerEffect("data/LittleQuest/Effect/DoublePunchSprite.efk", 1.5f);
    _powerDoublePunchEffect = LoadEffekseerEffect("data/LittleQuest/Effect/DoublePunchSprite2.efk", 1.5f);
    _pEffectList            = new int[4]{_punchEffect, _powerPunchEffect, _doublePunchEffect, _powerDoublePunchEffect};

    _bodyBox = AddComponent<ComponentCollisionCapsule>();
    _bodyBox.lock()->SetTranslate({0, 0, -4});
    _bodyBox.lock()->UseGravity();
    _bodyBox.lock()->SetHeight(30);
    _bodyBox.lock()->SetRadius(6.5);
    _bodyBox.lock()->SetMass(100.0f);
    _bodyBox.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);

    _leftHandBox = AddComponent<ComponentCollisionCapsule>();
    _leftHandBox.lock()->AttachToModel("mixamorig:LeftHand");
    _leftHandBox.lock()->SetTranslate({0, -50, 0});
    _leftHandBox.lock()->SetHeight(15.0f);
    _leftHandBox.lock()->SetRadius(7.4f);
    _leftHandBox.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
    _leftHandBox.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    _leftHandBox.lock()->Overlap(~(u32)ComponentCollision::CollisionGroup::NONE);

    _rightHandBox = AddComponent<ComponentCollisionCapsule>();
    _rightHandBox.lock()->AttachToModel("mixamorig:RightHand");
    _rightHandBox.lock()->SetTranslate({0, -50, 0});
    _rightHandBox.lock()->SetHeight(15.0f);
    _rightHandBox.lock()->SetRadius(7.4f);
    _rightHandBox.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
    _rightHandBox.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    _rightHandBox.lock()->Overlap(~(u32)ComponentCollision::CollisionGroup::NONE);

    _componentHP = AddComponent<ComponentHP>();
    _componentHP.lock()->SetType(ComponentHP::HP_TYPE::BOSS);
    _componentHP.lock()->SetHP(MAX_HP);

    _MawJLaygoCombo = MawJLaygoCombo::NONE;
    _state          = MawJLaygoState::IDLE;
    _player         = Scene::GetObjectPtr<Player>("Player");

    _attackSE = LoadSoundMem("data/LittleQuest/Audio/SE/MawJLaygoAttack.wav");

    _waitTime = NORMAL_WAIT;

    srand((unsigned)time(NULL));

    return Super::Init();
}

void MawJLaygo::Update() {
#if 0
    return;
#endif    // _DEBUG

    switch(_sceneState) {
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

void MawJLaygo::TransInAction() {
    switch(_state) {
    case MawJLaygoState::IDLE:
        _model.lock()->PlayAnimationNoSame(STR(MawJLaygoState::IDLE));
        break;
    case MawJLaygoState::TAUNT:
        Taunt();
        break;
    }
}

void MawJLaygo::GameAction() {
    if(_damageTimer > 0) {
        _damageTimer -= GetDeltaTime60();
    } else {
        _damageCount = 0;
    }

    if(_MawJLaygoCombo == MawJLaygoCombo::NONE && _state == MawJLaygoState::IDLE) {
        if(!_isAngry) {
            SelectAction();
        } else {
            SelectAngryAction();
        }

        if(_componentHP.lock()->GetHPRate() < 50.0f && !_isAngry) {
            ChangeState(MawJLaygoState::ANGRY);
        }
    }
    switch(_state) {
    case MawJLaygoState::IDLE:
        Idle();
        break;
    case MawJLaygoState::ATTACK:
        Attack();
        break;
    case MawJLaygoState::WAIT:
        Wait();
        break;
    case MawJLaygoState::TURN_LEFT:
        _model.lock()->PlayAnimationNoSame(STR(MawJLaygoState::TURN_LEFT));
        if(!_model.lock()->IsPlaying()) {
            ChangeState(MawJLaygoState::WAIT);
        } else {
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 1);
        }
        break;
    case MawJLaygoState::TURN_RIGHT:
        _model.lock()->PlayAnimationNoSame(STR(MawJLaygoState::TURN_RIGHT));
        if(!_model.lock()->IsPlaying()) {
            ChangeState(MawJLaygoState::WAIT);
        } else {
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 1);
        }
        break;
    case MawJLaygoState::CHASE:
        ChasePlayer();
        break;
    case MawJLaygoState::ANGRY:
        PowerUp();
        break;
    case MawJLaygoState::GET_HIT:
        Damaging();
        break;
    case MawJLaygoState::DEAD:
        Die();
        break;
    }
}

void MawJLaygo::TransOutAction() {
    switch(_state) {
    case MawJLaygoState::DEAD:
        Die();
        break;
    default:
        Idle();
        break;
    }
}

void MawJLaygo::LateDraw() {
    if(Scene::IsEdit()) {}
    switch(_sceneState) {
    case Scene::SceneState::TRANS_IN:
        break;
    case Scene::SceneState::GAME:
        if(!_hideUI) {
            _componentHP.lock()->DrawHPBar();
        }
        break;
    case Scene::SceneState::TRANS_OUT:
        break;
    }
}

void MawJLaygo::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    if((u32)hitInfo.collision_->GetCollisionGroup() & (u32)ComponentCollision::CollisionGroup::ENEMY_WEAPON) {
        auto* owner = hitInfo.hit_collision_->GetOwner();
        if(auto player = dynamic_cast<Player*>(owner)) {
            if(!_isHitPlayer) {
                _isHitPlayer = true;
                player->GetHit((int)(_comboList[_MawJLaygoCombo] * _attackVal * (1 + _isAngry)));
            }
        }
    }

    Super::OnHit(hitInfo);
}

void MawJLaygo::Idle() {
    _model.lock()->PlayAnimationNoSame(STR(MawJLaygoState::IDLE), true);
}

void MawJLaygo::Wait() {
    _MawJLaygoCombo = MawJLaygoCombo::NONE;
    _combo          = 0;
    _model.lock()->PlayAnimationNoSame(STR(MawJLaygoState::IDLE), true, 0.3f);
    _waitFor -= GetDeltaTime60();

    if(_waitFor <= 0.0f || (_componentHP.lock()->GetHPRate() < 50.0f && !_isAngry)) {
        ChangeState(MawJLaygoState::IDLE);
    }
}

void MawJLaygo::ChasePlayer() {
    float3 move     = _player.lock()->GetTranslate() - this->GetTranslate();
    float  distance = GetDistance(move);

    _model.lock()->PlayAnimationNoSame("Walk", true);

    if(distance > 0) {
        move = normalize(move);
        SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 10);
        move *= WALK_SPEED * GetDeltaTime60();
        AddTranslate(move);
    }

    if(distance < CLOSE_DISTANCE) {
        ChangeState(MawJLaygoState::IDLE);
    }
}

void MawJLaygo::SelectAction() {
    float  distance  = GetDistance(this->GetTranslate(), _player.lock()->GetTranslate());
    float  angle     = GetDegreeToPosition(_player.lock()->GetTranslate());
    int    random    = rand() % 100 + 1;
    float3 front     = GetMatrix().axisZ() - GetTranslate();
    float  dotPlayer = VDot(cast(front), cast(_player.lock()->GetTranslate()));

    if(angle >= FRONT_ANGLE && angle <= BACK_ANGLE) {
        if(dotPlayer < 0) {
            ChangeState(MawJLaygoState::TURN_RIGHT);
        } else {
            ChangeState(MawJLaygoState::TURN_LEFT);
        }
    } else if(distance < TOO_CLOSE_DISTANCE && angle < FRONT_ANGLE) {
        ChangeState(MawJLaygoState::ATTACK);
        if(random <= 35) {
            _MawJLaygoCombo = MawJLaygoCombo::BACKFLIP_PUNCH;
        } else if(random <= 70) {
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 30);
            _MawJLaygoCombo = MawJLaygoCombo::SWIP;
        } else if(random <= 80) {
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 50);
            _MawJLaygoCombo = MawJLaygoCombo::COMBO5;
        } else {
            if(dotPlayer < 0) {
                ChangeState(MawJLaygoState::TURN_RIGHT);
            } else {
                ChangeState(MawJLaygoState::TURN_LEFT);
            }
        }
        _combo = 1;
    } else if(distance < CLOSE_DISTANCE && angle < FRONT_ANGLE) {
        ChangeState(MawJLaygoState::ATTACK);
        if(random <= 90) {
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 30);
            _MawJLaygoCombo = MawJLaygoCombo::SWIP;
        } else {
            if(dotPlayer < 0) {
                ChangeState(MawJLaygoState::TURN_RIGHT);
            } else {
                ChangeState(MawJLaygoState::TURN_LEFT);
            }
        }
        _combo = 1;
    } else if(distance < MIDDLE_DISTANCE && angle > BACK_ANGLE) {
        if(random <= 30) {
            ChangeState(MawJLaygoState::ATTACK);
            _MawJLaygoCombo = MawJLaygoCombo::BACKFLIP_PUNCH;
            _combo          = 1;
        } else {
            if(dotPlayer < 0) {
                ChangeState(MawJLaygoState::TURN_RIGHT);
            } else {
                ChangeState(MawJLaygoState::TURN_LEFT);
            }
        }
    } else if(distance >= CLOSE_DISTANCE && distance <= FAR_DISTANCE) {
        ChangeState(MawJLaygoState::CHASE);
    } else if(distance > FAR_DISTANCE) {
        ChangeState(MawJLaygoState::ATTACK);
        if(random <= 90) {
            SetRotationToPosition(_player.lock()->GetTranslate());
            _MawJLaygoCombo = MawJLaygoCombo::CHARGE_PUNCH;
            _combo          = 1;
        } else {
            ChangeState(MawJLaygoState::CHASE);
        }
    }
}

void MawJLaygo::SelectAngryAction() {
    float  distance  = GetDistance(this->GetTranslate(), _player.lock()->GetTranslate());
    float  angle     = GetDegreeToPosition(_player.lock()->GetTranslate());
    int    random    = rand() % 100 + 1;
    float3 front     = GetMatrix().axisZ() - GetTranslate();
    float  dotPlayer = VDot(cast(front), cast(_player.lock()->GetTranslate()));

    if(angle >= FRONT_ANGLE && angle <= BACK_ANGLE) {
        if(dotPlayer < 0) {
            ChangeState(MawJLaygoState::TURN_RIGHT);
        } else {
            ChangeState(MawJLaygoState::TURN_LEFT);
        }
    } else if(distance < TOO_CLOSE_DISTANCE && angle < FRONT_ANGLE) {
        ChangeState(MawJLaygoState::ATTACK);
        if(random <= 40) {
            _MawJLaygoCombo = MawJLaygoCombo::BACKFLIP_PUNCH;
        } else if(random <= 70) {
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 50);
            _MawJLaygoCombo = MawJLaygoCombo::COMBO5;
        } else if(random <= 90) {
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 30);
            _MawJLaygoCombo = MawJLaygoCombo::SWIP;
        } else {
            if(dotPlayer < 0) {
                ChangeState(MawJLaygoState::TURN_RIGHT);
            } else {
                ChangeState(MawJLaygoState::TURN_LEFT);
            }
        }
        _combo = 1;
    } else if(distance < CLOSE_DISTANCE && angle < FRONT_ANGLE) {
        ChangeState(MawJLaygoState::ATTACK);
        if(random <= 45) {
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 50);
            _MawJLaygoCombo = MawJLaygoCombo::COMBO5;
        } else {
            if(dotPlayer < 0) {
                ChangeState(MawJLaygoState::TURN_RIGHT);
            } else {
                ChangeState(MawJLaygoState::TURN_LEFT);
            }
        }
        _combo = 1;
    } else if(distance < MIDDLE_DISTANCE && angle > BACK_ANGLE) {
        if(random <= 90) {
            ChangeState(MawJLaygoState::ATTACK);
            _MawJLaygoCombo = MawJLaygoCombo::BACKFLIP_PUNCH;
            _combo          = 1;
        } else {
            if(dotPlayer < 0) {
                ChangeState(MawJLaygoState::TURN_RIGHT);
            } else {
                ChangeState(MawJLaygoState::TURN_LEFT);
            }
        }
    } else if(distance >= CLOSE_DISTANCE && distance <= FAR_DISTANCE) {
        ChangeState(MawJLaygoState::CHASE);
    } else if(distance > FAR_DISTANCE) {
        ChangeState(MawJLaygoState::ATTACK);
        if(random <= 90) {
            SetRotationToPosition(_player.lock()->GetTranslate());
            _MawJLaygoCombo = MawJLaygoCombo::CHARGE_PUNCH;
            _combo          = 1;
        } else {
            ChangeState(MawJLaygoState::CHASE);
        }
    }
}

void MawJLaygo::Attack() {
    switch(_MawJLaygoCombo) {
    case MawJLaygoCombo::COMBO5:
        Combo5();
        break;
    case MawJLaygoCombo::BACKFLIP_PUNCH:
        BackflipPunch();
        break;
    case MawJLaygoCombo::CHARGE_PUNCH:
        ChargePunch();
        break;
    case MawJLaygoCombo::SWIP:
        Swip();
        break;
    }
}

void MawJLaygo::AttackAnimation(std::string animName, AnimInfo& animInfo, std::vector<ComponentCollisionCapsulePtr> atkCol,
                                bool playSE) {
    if(_model.lock()->GetPlayAnimationName() != animName) {
        _model.lock()->PlayAnimationNoSame(animName, false, 0.2F, animInfo.animStartTime);
        _model.lock()->SetAnimationSpeed(animInfo.animStartSpeed);
        _playedSE = false;
    }
    _currAnimTime = _model.lock()->GetAnimationPlayTime();
    if(_currAnimTime >= animInfo.triggerStartTime) {
        if(_slowMotion) {
            _model.lock()->SetAnimationSpeed(animInfo.animSpeed * 0.01f);
        } else {
            _model.lock()->SetAnimationSpeed(animInfo.animSpeed);
        }
        for(int i = 0; i < atkCol.size(); i++) {
            atkCol[i]->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);
        }

        if(!_playedSE && playSE) {
            PlaySoundMem(_attackSE, DX_PLAYTYPE_BACK);
            ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), _attackSE);
            _playedSE = true;
        }
    }
    if(_currAnimTime >= animInfo.triggerEndTime) {
        for(int i = 0; i < atkCol.size(); i++) {
            atkCol[i]->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
        }
    }
    if(_currAnimTime >= animInfo.animCutInTime) {
        _combo++;
        _isHitPlayer = false;
    }
}

void MawJLaygo::AttackAnimation(std::string animName, AnimInfo& animInfo, bool playSE) {
    AttackAnimation(animName, animInfo, {}, playSE);
}

void MawJLaygo::Combo5() {
    switch(_combo) {
    case 1:
        AttackAnimation(STR(MawJLaygoAnim::SWIP_ATTACK), _animList[STR(MawJLaygoAnim::SWIP_ATTACK)], {_leftHandBox.lock()});
        if(_currAnimTime >= _animList[STR(MawJLaygoAnim::SWIP_ATTACK)].triggerStartTime) {
            _playingEffect = PlayEffekseer3DEffect(_pEffectList[0 + _isAngry]);
            SetPosPlayingEffekseer3DEffect(_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, -30 * DegToRad);
        }
        break;
    case 3:
    case 5:
        if(_isAngry) {
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 1);
        }
        AttackAnimation(STR(MawJLaygoAnim::SWIP_ATTACK), _animList[STR(MawJLaygoAnim::QUICK_SWIP)], {_leftHandBox.lock()});
        if(_currAnimTime >= _animList[STR(MawJLaygoAnim::QUICK_SWIP)].triggerStartTime) {
            _playingEffect = PlayEffekseer3DEffect(_pEffectList[0 + _isAngry]);
            SetPosPlayingEffekseer3DEffect(_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, -80 * DegToRad);
        }
        break;
    case 2:
    case 4:
        if(_isAngry) {
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 1);
        }
        AttackAnimation(STR(MawJLaygoAnim::PUNCH), _animList[STR(MawJLaygoAnim::QUICK_PUNCH)], {_rightHandBox.lock()});
        if(_currAnimTime >= _animList[STR(MawJLaygoAnim::QUICK_PUNCH)].triggerStartTime) {
            _playingEffect = PlayEffekseer3DEffect(_pEffectList[0 + _isAngry]);
            SetPosPlayingEffekseer3DEffect(_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, 80 * DegToRad);
        }
        break;
    default:
        ChangeState(MawJLaygoState::WAIT);
        _waitFor = _waitTime;
        break;
    }
}

void MawJLaygo::BackflipPunch() {
    auto   pos = GetTranslate();
    float3 vec = GetMatrix().axisZ();
    vec.y      = 0;
    switch(_combo) {
    case 1:
        AttackAnimation(STR(MawJLaygoAnim::BACKFLIP), _animList[STR(MawJLaygoAnim::BACKFLIP)], true);
        vec *= 1.0f * GetDeltaTime60();
        AddTranslate(vec);
        if(_currAnimTime < _animList[STR(MawJLaygoAnim::BACKFLIP)].animCutInTime) {
            _bodyBox.lock()->Overlap((u32)ComponentCollision::CollisionGroup::PLAYER);
        } else {
            _bodyBox.lock()->Overlap(!(u32)ComponentCollision::CollisionGroup::PLAYER);
        }
        break;
    case 2:
        AttackAnimation(STR(MawJLaygoAnim::DOUBLE_PUNCH), _animList[STR(MawJLaygoAnim::DOUBLE_PUNCH)],
                        {_leftHandBox.lock(), _rightHandBox.lock()});
        if(_currAnimTime < _animList[STR(MawJLaygoAnim::DOUBLE_PUNCH)].triggerStartTime) {
            float distance = GetDistance(this->GetTranslate(), _player.lock()->GetTranslate());
            vec *= distance * -0.25f * GetDeltaTime60();
            AddTranslate(vec);
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 10);
        } else {
            _playingEffect = PlayEffekseer3DEffect(_pEffectList[2 + _isAngry]);
            SetPosPlayingEffekseer3DEffect(_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, 0);
        }
        break;
    default:
        ChangeState(MawJLaygoState::WAIT);
        _waitFor = _waitTime;
        break;
    }
}

void MawJLaygo::ChargePunch() {
    auto   pos = GetTranslate();
    float3 vec = GetMatrix().axisZ();
    vec.y      = 0;
    switch(_combo) {
    case 1:
        AttackAnimation(STR(MawJLaygoAnim::CHARGE), _animList[STR(MawJLaygoAnim::CHARGE)], false);
        break;
    case 2:
        AttackAnimation(STR(MawJLaygoAnim::DOUBLE_PUNCH), _animList[STR(MawJLaygoAnim::DOUBLE_PUNCH)],
                        {_leftHandBox.lock(), _rightHandBox.lock()});
        if(_currAnimTime < _animList[STR(MawJLaygoAnim::DOUBLE_PUNCH)].triggerStartTime) {
            float distance = GetDistance(this->GetTranslate(), _player.lock()->GetTranslate());
            vec *= distance * -0.25f * GetDeltaTime60();
            AddTranslate(vec);
            SetRotationToPositionWithLimit(_player.lock()->GetTranslate(), 10);
        } else {
            _playingEffect = PlayEffekseer3DEffect(_pEffectList[2 + _isAngry]);
            SetPosPlayingEffekseer3DEffect(_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, 0);
        }
        break;
    default:
        ChangeState(MawJLaygoState::WAIT);
        _waitFor = _waitTime;
        break;
    }
}

void MawJLaygo::Swip() {
    float distance;
    switch(_combo) {
    case 1:
        AttackAnimation(STR(MawJLaygoAnim::SWIP_ATTACK), _animList[STR(MawJLaygoAnim::SWIP_ATTACK)], {_leftHandBox.lock()});
        if(_currAnimTime >= _animList[STR(MawJLaygoAnim::SWIP_ATTACK)].triggerStartTime) {
            _playingEffect = PlayEffekseer3DEffect(_pEffectList[0 + _isAngry]);
            SetPosPlayingEffekseer3DEffect(_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                           GetTranslate().y + 12,
                                           GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
            SetRotationPlayingEffekseer3DEffect(_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, -30 * DegToRad);
        }
        break;
    case 2:
        distance = GetDistance(this->GetTranslate(), _player.lock()->GetTranslate());
        if(distance < 50) {
            AttackAnimation(STR(MawJLaygoAnim::PUNCH), _animList[STR(MawJLaygoAnim::PUNCH)], {_rightHandBox.lock()});
            if(_currAnimTime >= _animList[STR(MawJLaygoAnim::PUNCH)].triggerStartTime) {
                _playingEffect = PlayEffekseer3DEffect(_pEffectList[0 + _isAngry]);
                SetPosPlayingEffekseer3DEffect(_playingEffect, GetTranslate().x - 15 * sinf(GetRotationAxisXYZ().y * DegToRad),
                                               GetTranslate().y + 12,
                                               GetTranslate().z - 15 * cosf(GetRotationAxisXYZ().y * DegToRad));
                SetRotationPlayingEffekseer3DEffect(_playingEffect, 0, GetRotationAxisXYZ().y * DegToRad, 80 * DegToRad);
            }
        } else {
            _combo++;
        }
        break;
    default:
        ChangeState(MawJLaygoState::WAIT);
        _waitFor = _waitTime;
        break;
    }
}

void MawJLaygo::RangedShot() {
    switch(_combo) {
    case 1:
        _combo++;
        break;
    default:
        ChangeState(MawJLaygoState::WAIT);
        _waitFor = _waitTime;
        break;
    }
}

void MawJLaygo::ChargeExplode() {
    switch(_combo) {
    case 1:
        AttackAnimation(STR(MawJLaygoAnim::EXPLODE_CHARGE), _animList[STR(MawJLaygoAnim::EXPLODE_CHARGE)],
                        {_leftHandBox.lock()});
        break;
    default:
        ChangeState(MawJLaygoState::WAIT);
        _waitFor = _waitTime;
        break;
    }
}

void MawJLaygo::Taunt() {
    switch(_combo) {
    case 1:
        AttackAnimation(STR(MawJLaygoAnim::TAUNT_ANIM), _animList[STR(MawJLaygoAnim::TAUNT_ANIM)], false);
        break;
    default:
        ChangeState(MawJLaygoState::IDLE);
        break;
    }
}

void MawJLaygo::PowerUp() {
    _model.lock()->PlayAnimationNoSame(STR(MawJLaygoAnim::CHARGE));
    _currAnimTime = _model.lock()->GetAnimationPlayTime();
    if(_currAnimTime > _animList[STR(MawJLaygoAnim::ANGRY_AURA)].triggerStartTime) {
        _model.lock()->SetAnimationSpeed(_animList[STR(MawJLaygoAnim::ANGRY_AURA)].animSpeed);
        if(_angryBox.expired()) {
            _angryBox = AddComponent<ComponentCollisionSphere>();
            _angryBox.lock()->SetTranslate({0, 0, 0});
            _angryBox.lock()->SetRadius(20.0f);
            _angryBox.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
            _angryBox.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);
            _angryBox.lock()->Overlap(~(u32)ComponentCollision::CollisionGroup::NONE);
        }

        if(IsEffekseer3DEffectPlaying(_playingEffect) == -1 && !_isAngry) {
            _isAngry       = true;
            _playingEffect = PlayEffekseer3DEffect(_powerUpEffect);
            SetPosPlayingEffekseer3DEffect(_playingEffect, GetTranslate().x, GetTranslate().y, GetTranslate().z);
        }
    }
    if(_currAnimTime > _animList[STR(MawJLaygoAnim::ANGRY_AURA)].triggerEndTime) {
        if(!_angryBox.expired()) {
            RemoveComponent(_angryBox.lock());
            _angryBox.reset();
            StopEffekseer3DEffect(_playingEffect);
        }
    }
    if(_currAnimTime > _animList[STR(MawJLaygoAnim::ANGRY_AURA)].animCutInTime) {
        _combo++;
        _isHitPlayer = false;
    }

    _waitTime = ANGRY_WAIT;

    if(!_model.lock()->IsPlaying()) {
        ChangeState(MawJLaygoState::WAIT);
        _waitFor = _waitTime;
    }
}

void MawJLaygo::GetHit(int damage) {
    _componentHP.lock()->TakeDamage(damage);
    _damageCount += damage;

    if(_componentHP.lock()->GetHP() <= 0) {
        ChangeState(MawJLaygoState::DEAD);
        _MawJLaygoCombo = MawJLaygoCombo::NONE;
        _combo          = 1;
        return;
    }

    if(_damageTimer <= 0) {
        _damageTimer = DAMAGE_TIME;
    }

    if(_damageCount > _damegeCap && _state != MawJLaygoState::ANGRY) {
        ChangeState(MawJLaygoState::GET_HIT);
        _MawJLaygoCombo = MawJLaygoCombo::NONE;
        _combo          = 1;
        _damageCount    = 0;
    }
}

void MawJLaygo::Damaging() {
    switch(_combo) {
    case 1:
        AttackAnimation(STR(MawJLaygoState::GET_HIT), _animList[STR(MawJLaygoState::GET_HIT)], false);
        break;
    default:
        ChangeState(MawJLaygoState::WAIT);
        _waitFor = _waitTime;
        break;
    }
}

void MawJLaygo::Die() {
    _model.lock()->PlayAnimationNoSame(STR(MawJLaygoState::DEAD));
    if(_slowMotion) {
        _model.lock()->SetAnimationSpeed(0.1f);
    } else {
        _model.lock()->SetAnimationSpeed(1.0f);
    }

    if(_bodyBox.lock()) {
        RemoveComponent(_bodyBox.lock());
        _bodyBox.reset();
    }

    if(!_angryBox.expired()) {
        RemoveComponent(_angryBox.lock());
        _angryBox.reset();
    }
}

void MawJLaygo::PlayTaunt() {
    ChangeState(MawJLaygoState::TAUNT);
    _combo = 1;
}

void MawJLaygo::PlayDead() {
    ChangeState(MawJLaygoState::DEAD);
}

bool MawJLaygo::IsPlayedTaunt() {
    if(_model.lock()->GetOldPlayAnimationName() == STR(MawJLaygoAnim::TAUNT_ANIM)) {
        ChangeState(MawJLaygoState::WAIT);
        return true;
    }
    return false;
}

bool MawJLaygo::IsDead() {
    return _componentHP.lock()->GetHP() <= 0;
}

void MawJLaygo::SlowMotion() {
    _slowMotion = true;
}

void MawJLaygo::EndSlowMotion() {
    _slowMotion = false;
}

void MawJLaygo::ChangeState(MawJLaygoState state) {
    this->_state = state;
}

void MawJLaygo::SetSceneState(Scene::SceneState state) {
    _sceneState = state;
    ChangeState(MawJLaygoState::IDLE);
}

void MawJLaygo::Exit() {
    delete[] _pEffectList;

    DeleteSoundMem(_attackSE);
    DeleteEffekseerEffect(_powerUpEffect);
    DeleteEffekseerEffect(_punchEffect);
    DeleteEffekseerEffect(_powerPunchEffect);
    DeleteEffekseerEffect(_doublePunchEffect);
    DeleteEffekseerEffect(_powerDoublePunchEffect);
}

void MawJLaygo::SetAnimList() {
    AnimInfo info         = {};
    info.triggerStartTime = 75;
    info.triggerEndTime   = 83;
    info.animCutInTime    = 83;
    info.animSpeed        = 1.2f;
    info.animStartSpeed   = 2.0f;

    _animList[STR(MawJLaygoAnim::SWIP_ATTACK)] = info;

    info.animStartSpeed = 1.0f;
    info.animStartTime  = 75;
    info.animCutInTime  = 83;

    _animList[STR(MawJLaygoAnim::QUICK_SWIP)] = info;

    info                  = {};
    info.triggerStartTime = 15;
    info.triggerEndTime   = 29;
    info.animCutInTime    = 35;
    info.animStartSpeed   = 1.0f;
    info.animSpeed        = 1.0f;

    _animList[STR(MawJLaygoAnim::PUNCH)] = info;

    info.animStartTime  = 15;
    info.triggerEndTime = 20;
    info.animCutInTime  = 20;

    _animList[STR(MawJLaygoAnim::QUICK_PUNCH)] = info;

    info               = {};
    info.animStartTime = 27;
    info.animCutInTime = 90;
    info.animSpeed     = 2.0f;

    _animList[STR(MawJLaygoAnim::BACKFLIP)] = info;

    info                  = {};
    info.animStartTime    = 100;
    info.triggerStartTime = 124;
    info.triggerEndTime   = 134;
    info.animCutInTime    = 190;
    info.animSpeed        = 2.0f;
    info.animStartSpeed   = 6.0f;

    _animList[STR(MawJLaygoAnim::DOUBLE_PUNCH)] = info;

    info                  = {};
    info.triggerStartTime = 20;
    info.animCutInTime    = 46;
    info.animStartSpeed   = 0.8f;
    info.animSpeed        = 1.0f;

    _animList[STR(MawJLaygoAnim::CHARGE)] = info;

    info                  = {};
    info.triggerStartTime = 205;
    info.animStartSpeed   = 0.5f;
    info.animSpeed        = 1.0f;

    _animList[STR(MawJLaygoAnim::EXPLODE_CHARGE)] = info;

    info                  = {};
    info.triggerStartTime = 27;
    info.triggerEndTime   = 105;
    info.animCutInTime    = 170;
    info.animStartSpeed   = 0.5f;

    _animList[STR(MawJLaygoAnim::ANGRY_AURA)] = info;

    info                = {};
    info.animCutInTime  = 170;
    info.animStartSpeed = 2.0f;
    info.animSpeed      = 2.0f;

    _animList[STR(MawJLaygoAnim::TAUNT_ANIM)] = info;

    info               = {};
    info.animCutInTime = 167;

    _animList[STR(MawJLaygoState::GET_HIT)] = info;
}

void MawJLaygo::SetComboList() {
    _comboList[MawJLaygoCombo::SWIP]           = 1;
    _comboList[MawJLaygoCombo::COMBO5]         = 0.7f;
    _comboList[MawJLaygoCombo::BACKFLIP_PUNCH] = 1;
    _comboList[MawJLaygoCombo::CHARGE_PUNCH]   = 2;
    _comboList[MawJLaygoCombo::CHARGE_EXPLODE] = 5;
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::MawJLaygo)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::MawJLaygo)

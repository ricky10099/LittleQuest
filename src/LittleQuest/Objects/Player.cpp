#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "LittleQuest/Components/ComponentCombo.h"
#include "LittleQuest/Scenes/Stage01.h"
#include "LittleQuest/Objects/BreakableObject.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {

static std::string_view colName = "";
PlayerPtr               Player::Create(const float3& pos) {
    auto player = Scene::CreateObjectPtr<Player>();

    player->SetName("Player");
    player->SetTranslate(pos);

    return player;
}

bool Player::Init() {
    _model = AddComponent<ComponentModel>("data/LittleQuest/Model/Kachujin/Kachujin.mv1");
    _model.lock()->SetScaleAxisXYZ({0.05f});
    _model.lock()->SetRotationAxisXYZ({0, -90, 0});
    _model.lock()->SetAnimation({
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
        {     STR(Combo::EXSKILL_0),         "data/LittleQuest/Anim/Player/EXSkill_0.mv1", 0, 7.5f},
        {     STR(Combo::EXSKILL_1),         "data/LittleQuest/Anim/Player/EXSkill_1.mv1", 0, 1.0f},
        { STR(PlayerState::GET_HIT),    "data/LittleQuest/Anim/KachujinSet/HitToBody.mv1", 0, 3.0f},
    });
    _model.lock()->PlayAnimationNoSame(STR(PlayerState::IDLE), true);

    SetAnimInfo();
    SetComboList();

    {
        auto sword = Scene::CreateObjectPtr<Object>("Katana");
        auto model = sword->AddComponent<ComponentModel>();
        model->Load("data/LittleQuest/Model/Katana/Katana.mv1");
        model->SetRotationAxisXYZ({0, -10, 0});
        model->SetScaleAxisXYZ({3.5f, 4.5f, 4.5f});

        auto attach = sword->AddComponent<ComponentAttachModel>();
        attach->SetAttachObject(shared_from_this(), "mixamorig:RightHand");
        attach->SetAttachRotate({175, 0, -90});
        attach->SetAttachOffset({-65, 13, -4});
    }
    _componentHP = AddComponent<ComponentHP>();
    _componentHP.lock()->SetType(ComponentHP::HP_TYPE::PLAYER);
    _componentHP.lock()->SetHP(MAX_HP);

    _componentCombo = AddComponent<ComponentCombo>();

    auto colCap = AddComponent<ComponentCollisionCapsule>();
    colCap->SetTranslate({0, 0.5f, 0});
    colCap->SetRadius(1.1f);
    colCap->SetHeight(10.5);
    colCap->UseGravity();
    colCap->SetCollisionGroup(ComponentCollision::CollisionGroup::PLAYER);

    _weaponCollision = AddComponent<ComponentCollisionCapsule>();
    _weaponCollision.lock()->AttachToModel("mixamorig:RightHand");
    _weaponCollision.lock()->SetTranslate({10, 13, -4});
    _weaponCollision.lock()->SetRotationAxisXYZ({0, 0, -92});
    _weaponCollision.lock()->SetRadius(0.2f);
    _weaponCollision.lock()->SetHeight(10.0f);
    _weaponCollision.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::WEAPON);
    _weaponCollision.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    _weaponCollision.lock()->Overlap((u32)ComponentCollision::CollisionGroup::ENEMY |
                                     (u32)ComponentCollision::CollisionGroup::ITEM);

    _exSkillCollision = AddComponent<ComponentCollisionSphere>();
    _exSkillCollision.lock()->SetRadius(20.0f);
    _exSkillCollision.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::WEAPON);
    _exSkillCollision.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    _exSkillCollision.lock()->Overlap((u32)ComponentCollision::CollisionGroup::ENEMY);

    _hitEffect         = LoadEffekseerEffect("data/LittleQuest/Effect/LossOfBlood.efk", 0.5f);
    _hitBuildingEffect = LoadEffekseerEffect("data/LittleQuest/Effect/HitBuilding.efk", 2.0f);
    _slashEffect1      = LoadEffekseerEffect("data/LittleQuest/Effect/SwordSlashSprite1.efk", 5.0f);
    _slashEffect2      = LoadEffekseerEffect("data/LittleQuest/Effect/SwordSlashSprite2.efk", 5.0f);
    _slashEffect3      = LoadEffekseerEffect("data/LittleQuest/Effect/SwordSlashSprite3.efk", 5.0f);
    _pEffectList       = new int[3]{_slashEffect1, _slashEffect2, _slashEffect3};
    _chargingEffect    = LoadEffekseerEffect("data/LittleQuest/Effect/Charging.efk", 2.5f);
    _chargedEffect     = LoadEffekseerEffect("data/LittleQuest/Effect/Charged.efk", 2.5f);
    _pChargeList       = new int[2]{_chargingEffect, _chargedEffect};
    _exSkillEffect     = LoadEffekseerEffect("data/LittleQuest/Effect/BloodLance.efk", 2.0f);

    _selfMatrix  = GetMatrix();
    _speedFactor = RUN_MULTIPLIER;

    _swordSE    = LoadSoundMem("data/LittleQuest/Audio/SE/sword-swipes-2-quick.mp3");
    _swordHitSE = LoadSoundMem("data/LittleQuest/Audio/SE/SwordHit.wav");
    _exSkill0SE = LoadSoundMem("data/LittleQuest/Audio/SE/EXSkill_0.mp3");
    _exSkill1SE = LoadSoundMem("data/LittleQuest/Audio/SE/EXSkill_1.mp3");

    _cameraCorrection = AddComponent<ComponentCollisionLine>();
    _cameraCorrection.lock()->SetTranslate({0, 0, 0});
    _cameraCorrection.lock()->SetHitCollisionGroup(
        (u32)ComponentCollision::CollisionGroup::GROUND | (u32)ComponentCollision::CollisionGroup::WALL |
        (u32)ComponentCollision::CollisionGroup::ITEM | (u32)ComponentCollision::CollisionGroup::CAMERA);
    _cameraCorrection.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
    _cameraCorrection.lock()->SetName("CamCorrection");

    auto obj       = Scene::CreateObjectPtr<Object>()->SetName("EXSkillCamera");
    _exSkillCamera = obj->AddComponent<ComponentCamera>();

    return Super::Init();
}

// ゲーム中と演出の行動を分ける
void Player::Update() {
    switch(_sceneState) {
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
    _movement = float3(0, 0, 0);
    _hitTimer -= GetDeltaTime60();
    _hitTimer = std::max(0.0f, _hitTimer);

    if(_hitTimer <= 0 && _isHit) {
        _isHit = false;
    }

    if(!_camera.lock()) {
        _camera       = Scene::GetObjectPtr<Camera>("PlayerCamera");
        _cameraLength = _camera.lock()->GetComponent<ComponentSpringArm>()->GetSpringArmLength();
    } else {
        float3 v    = _camera.lock()->CameraForward();
        _selfMatrix = HelperLib::Math::CreateMatrixByFrontVector(v);
        float3 dir  = _camera.lock()->GetTranslate() - this->GetTranslate();
        dir         = normalize(dir);
        _cameraCorrection.lock()->SetLine({0, 5, 0}, dir * _cameraLength);
    }

    _exSkillCamera.lock()->SetPositionAndTarget(_camera.lock()->GetTranslate(), GetTranslate());

    InputHandle();

    if(_currCombo != Combo::NO_COMBO && _playerState != PlayerState::ROLL) {
        _playerState = PlayerState::ATTACK;
    }

    if(IsFloat3Zero(_movement) && _playerState != PlayerState::GET_HIT && _playerState != PlayerState::ATTACK &&
       _playerState != PlayerState::ROLL && _playerState != PlayerState::DEAD) {
        _playerState = PlayerState::IDLE;
    }

    switch(_playerState) {
    case PlayerState::GET_HIT:
        if(!_model.lock()->IsPlaying()) {
            _playerState = PlayerState::IDLE;
        }
        break;
    case PlayerState::ATTACK:
        Attack();
        break;
    case PlayerState::ROLL:
        _model.lock()->PlayAnimationNoSame(STR(PlayerState::ROLL));
        _currCombo = Combo::NO_COMBO;
        _weaponCollision.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
        if(_model.lock()->IsPlaying()) {
            _movement     = {};
            _isInvincible = true;
            float3 vec    = _model.lock()->GetMatrix().axisZ();
            vec.y         = 0;
            _movement += -vec;
            _movement *= 15.0f * GetDeltaTime60();
            AddTranslate(_movement);
        } else {
            _isInvincible = false;
            _playerState  = PlayerState::IDLE;
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
    _model.lock()->SetRotationAxisXYZ({0, -90, 0});
    switch(_playerState) {
    case PlayerState::DEAD:
        Die();
        break;
    default:
        Idle();
        break;
    }
}

void Player::LateDraw() {
    Super::LateDraw();

    switch(_sceneState) {
    case Scene::SceneState::TRANS_IN:
        break;
    case Scene::SceneState::GAME:
        if(!_isHideHP) {
            _componentCombo.lock()->DrawComboBar();
        }
        break;
    case Scene::SceneState::TRANS_OUT:
        break;
    }
}

void Player::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    if((u32)hitInfo.collision_->GetCollisionGroup() & (u32)ComponentCollision::CollisionGroup::WEAPON) {
        auto* owner = hitInfo.hit_collision_->GetOwner();

        Enemy* enemy;
        if((enemy = dynamic_cast<Enemy*>(owner)) && _currCombo != Combo::NO_COMBO) {
            bool inList = false;
            for(int i = 0; i < _attackList.size(); i++) {
                if(_attackList[i] == enemy->GetName().data()) {
                    inList = true;
                    break;
                }
            }

            if(!inList) {
                _attackList.push_back(enemy->GetName().data());
                enemy->GetHit((int)(this->BASE_ATK * _comboList[_currCombo] * _componentCombo.lock()->ComboBuff()));
                _isHit = true;
                _hitTimer += HIT_PAUSE;
                _componentCombo.lock()->AddCombo(_comboList[_currCombo]);
                _playingEffect = PlayEffekseer3DEffect(_hitEffect);
                PlaySoundMem(_swordHitSE, DX_PLAYTYPE_BACK);
                ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), _swordHitSE);
                SetPosPlayingEffekseer3DEffect(_playingEffect, hitInfo.hit_position_.x, hitInfo.hit_position_.y,
                                               hitInfo.hit_position_.z);
                _camera.lock()->SetCameraShake(15, 5);
                _camera.lock()->ShakeCamera();
            }
        }

        BreakableObject* breakableObject;
        if((breakableObject = dynamic_cast<BreakableObject*>(owner)) && _currCombo != Combo::NO_COMBO) {
            bool inList = false;
            for(int i = 0; i < _attackList.size(); i++) {
                if(_attackList[i] == breakableObject->GetName().data()) {
                    inList = true;
                    break;
                }
            }

            if(!inList) {
                _attackList.push_back(breakableObject->GetName().data());
                if(_currCombo == Combo::EXSKILL_1) {
                    breakableObject->GetHit(10);
                } else {
                    breakableObject->GetHit();
                }
                _isHit = true;
                _componentCombo.lock()->AddCombo(1);
                _playingEffect = PlayEffekseer3DEffect(_hitBuildingEffect);
                SetPosPlayingEffekseer3DEffect(_playingEffect, hitInfo.hit_position_.x, hitInfo.hit_position_.y,
                                               hitInfo.hit_position_.z);
                _camera.lock()->SetCameraShake(10, 5);
                _camera.lock()->ShakeCamera();
            }
        }
    }

    if(_camera.lock()) {
        if((u32)hitInfo.collision_->GetCollisionGroup() & (u32)ComponentCollision::CollisionGroup::ETC) {
            if(hitInfo.hit_) {
                _blockedDistance = GetDistance(this->GetTranslate(), hitInfo.hit_position_, true);
                if(_blockedDistance < _cameraLength &&
                   hitInfo.hit_collision_->GetName() != _camera.lock()->GetAnchorCollisionName()) {
                    _cameraBlocked = true;
                    _blockedName   = hitInfo.hit_collision_->GetOwner()->GetName().data();
                    _camera.lock()->SetCameraPositionAndTarget(
                        {_camera.lock()->GetCameraLocalPosition().x, _camera.lock()->GetCameraLocalPosition().y,
                         _cameraLength - _blockedDistance},
                        {0, 0, _cameraLength});
                } else {
                    _cameraBlocked = false;
                    _camera.lock()->SetCameraPositionAndTarget(
                        {_camera.lock()->GetCameraLocalPosition().x, _camera.lock()->GetCameraLocalPosition().y, 0},
                        {0, 0, _cameraLength});
                }
            }
        } else {
            _camera.lock()->SetCameraPositionAndTarget(
                {_camera.lock()->GetCameraLocalPosition().x, _camera.lock()->GetCameraLocalPosition().y, 0},
                {0, 0, _cameraLength});
            _cameraBlocked = false;
        }
    }

    Super::OnHit(hitInfo);
}

void Player::GetHit(int damage) {
    if(_isInvincible) {
        return;
    }

    _componentHP.lock()->TakeDamage(damage);
    StartJoypadVibration(DX_INPUT_PAD1, 250, 500, -1);

    if(_componentHP.lock()->GetHP() <= 0) {
        _playerState = PlayerState::DEAD;
        return;
    }

    _model.lock()->PlayAnimation(STR(PlayerState::GET_HIT));
    _weaponCollision.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    _playerState = PlayerState::GET_HIT;
    _currCombo   = Combo::NO_COMBO;
    _chargeTime  = 0.0f;
}

void Player::InputHandle() {
    DINPUT_JOYSTATE DInputState;
    _cameraLength -= GetMouseWheelRotVol() * 3;
    if(IsPadRepeat(PAD_ID::PAD_R)) {
        ++_cameraLength;
    }
    if(IsPadRepeat(PAD_ID::PAD_L)) {
        --_cameraLength;
    }
    _cameraLength = std::min((std::max(_cameraLength, 10.0f)), 100.0f);
    _camera.lock()->SetCameraLength(_cameraLength);

    if(_playerState != PlayerState::ROLL && _playerState != PlayerState::GET_HIT && _playerState != PlayerState::DEAD) {
        _speedFactor = IsKeyRepeat(KEY_INPUT_LSHIFT) ? RUN_MULTIPLIER : 1.0f;

        GetJoypadDirectInputState(DX_INPUT_PAD1, &DInputState);
        switch(GetJoypadType(DX_INPUT_PAD1)) {
        case DX_PADTYPE_DUAL_SENSE:
            _movement += float3{-DInputState.X, 0, DInputState.Y};
            _movement = mul(float4{_movement, 1.0f}, _selfMatrix).xyz;
            break;
        default:
            break;
        }

        if(IsKeyRepeat(KEY_INPUT_W)) {
            float3 vec = _selfMatrix.axisZ();
            vec.y      = 0;
            _movement += -vec;
        }
        if(IsKeyRepeat(KEY_INPUT_D)) {
            float3 vec = _selfMatrix.axisX();
            vec.y      = 0;
            _movement += -vec;
        }
        if(IsKeyRepeat(KEY_INPUT_S)) {
            float3 vec = _selfMatrix.axisZ();
            vec.y      = 0;
            _movement += vec;
        }
        if(IsKeyRepeat(KEY_INPUT_A)) {
            float3 vec = _selfMatrix.axisX();
            vec.y      = 0;
            _movement += vec;
        }

        if(IsMouseDown(MOUSE_INPUT_LEFT) || IsPadOn(PAD_ID::PAD_A)) {
            _playerState = PlayerState::ATTACK;

            // もし攻撃してない場合はコンボ１に行く
            // 攻撃しているかつチャージ攻撃ではないならコンボ中に判断する
            if(_currCombo == Combo::NO_COMBO) {
                _currCombo = Combo::NORMAL_COMBO1;
            } else if(_currCombo != Combo::SPECIAL_ATTACK) {
                _isCombo = true;
            }
        }

        // 右クリック長押ししたらチャージ攻撃
        if(IsMouseRepeat(MOUSE_INPUT_RIGHT, 1) || IsPadRepeat(PAD_ID::PAD_Y)) {
            _chargeTime += GetDeltaTime60();
            if(_chargeTime >= SPECIAL_CHARGE_TIME && !_charged) {
                StopEffekseer3DEffect(_playingChargeEffect);
                _charged = true;
            }
            // チャージ時間によってチャージエフェクトが変わる
            if(IsEffekseer3DEffectPlaying(_playingChargeEffect) == -1) {
                _playingChargeEffect = PlayEffekseer3DEffect(_pChargeList[(_chargeTime >= SPECIAL_CHARGE_TIME)]);
            }
            SetPosPlayingEffekseer3DEffect(_playingChargeEffect, GetTranslate().x, GetTranslate().y + 7, GetTranslate().z);
            _speedFactor = 1.0;
        } else if(_chargeTime > 0 && _currCombo == Combo::NO_COMBO) {
            StopEffekseer3DEffect(_playingChargeEffect);
            if(_chargeTime >= SPECIAL_CHARGE_TIME) {
                _isCombo = true;
            }
            _charged     = false;
            _playerState = PlayerState::ATTACK;
            _currCombo   = Combo::SPECIAL_ATTACK;
            _chargeTime  = 0;
            _speedFactor = RUN_MULTIPLIER;
        }

        if(_componentCombo.lock()->IsFullCharge() && IsKeyDown(KEY_INPUT_R)) {
            _playerState = PlayerState::ATTACK;

            if(_currCombo == Combo::NO_COMBO) {
                _currCombo = Combo::EXSKILL_0;
            }
        }

        if(!IsFloat3Zero(_movement)) {
            _playerState = PlayerState::WALK;
        }

        // 必殺技途中は回避できない
        if((IsKeyDown(KEY_INPUT_SPACE) || IsPadOn(PAD_ID::PAD_B)) && _currCombo != Combo::EXSKILL_0 &&
           _currCombo != Combo::EXSKILL_1) {
            _chargeTime = 0;
            _charged    = false;
            StopEffekseer3DEffect(_playingChargeEffect);
            _playerState = PlayerState::ROLL;
            this->SetModelRotation();
        }
    }
}

void Player::Idle() {
    _model.lock()->PlayAnimationNoSame(STR(PlayerState::IDLE), true, 0.5f);
}

void Player::Walk() {
    _movement = normalize(_movement);
    this->SetModelRotation();
    _movement *= BASE_SPEED * _speedFactor * GetDeltaTime();
    AddTranslate(_movement);

    _model.lock()->PlayAnimationNoSame(STR(PlayerState::WALK), true, 0.2f, 14.0f);
    _model.lock()->SetAnimationSpeed(GetDistance(_movement) * 5.0f);
}

void Player::Attack() {
    switch(_currCombo) {
    case Combo::NORMAL_COMBO1:
        AttackAnimation(STR(Combo::NORMAL_COMBO1), _animList[STR(Combo::NORMAL_COMBO1)], Combo::NORMAL_COMBO2);
        if(_currAnimTime > _animList[STR(Combo::NORMAL_COMBO1)].triggerStartTime) {
            if(!_playedFX) {
                float3 fxPosition = {GetTranslate().x, GetTranslate().y + 6, GetTranslate().z};
                float3 fxRotation = {0, (_model.lock()->GetRotationAxisXYZ().y) * DegToRad, 0};
                PlayAttackFX(_pEffectList[(int)_componentCombo.lock()->ComboBuff() - 1], fxPosition, fxRotation, _swordSE);
                _playedFX = true;
            }
        }
        break;
    case Combo::NORMAL_COMBO2:
        AttackAnimation(STR(Combo::NORMAL_COMBO2), _animList[STR(Combo::NORMAL_COMBO2)], Combo::NORMAL_COMBO3);
        if(_currAnimTime > _animList[STR(Combo::NORMAL_COMBO2)].triggerStartTime) {
            if(!_playedFX) {
                float3 fxPosition = {GetTranslate().x, GetTranslate().y + 6, GetTranslate().z};
                float3 fxRotation = {0, (_model.lock()->GetRotationAxisXYZ().y) * DegToRad, 180 * DegToRad};
                PlayAttackFX(_pEffectList[(int)_componentCombo.lock()->ComboBuff() - 1], fxPosition, fxRotation, _swordSE);
                _playedFX = true;
            }
        }
        break;
    case Combo::NORMAL_COMBO3:
        AttackAnimation(STR(Combo::NORMAL_COMBO3), _animList[STR(Combo::NORMAL_COMBO3)], Combo::NORMAL_COMBO4);
        if(_currAnimTime > _animList[STR(Combo::NORMAL_COMBO3)].triggerStartTime) {
            if(!_playedFX) {
                float3 fxPosition = {GetTranslate().x, GetTranslate().y + 6, GetTranslate().z};
                float3 fxRotation = {0, (_model.lock()->GetRotationAxisXYZ().y) * DegToRad, -50 * DegToRad};
                PlayAttackFX(_pEffectList[(int)_componentCombo.lock()->ComboBuff() - 1], fxPosition, fxRotation, _swordSE);
                _playedFX = true;
            }
        }
        break;
    case Combo::NORMAL_COMBO4:
        AttackAnimation(STR(Combo::NORMAL_COMBO4), _animList[STR(Combo::NORMAL_COMBO4)]);
        if(_currAnimTime > _animList[STR(Combo::NORMAL_COMBO4)].triggerStartTime) {
            if(!_playedFX) {
                float3 fxPosition = {GetTranslate().x, GetTranslate().y + 6, GetTranslate().z};
                float3 fxRotation = {0, (_model.lock()->GetRotationAxisXYZ().y) * DegToRad, 52 * DegToRad};
                PlayAttackFX(_pEffectList[(int)_componentCombo.lock()->ComboBuff() - 1], fxPosition, fxRotation, _swordSE);
                _playedFX = true;
            }
        }
        break;
    case Combo::SPECIAL_ATTACK:
        // チャージ完成したら２段攻撃になる
        if(_isCombo) {
            AttackAnimation(STR(Combo::SPECIAL_ATTACK), _animList[STR(Combo::SPECIAL_ATTACK)], Combo::SPECIAL_CHARGE);
        } else {
            AttackAnimation(STR(Combo::SPECIAL_ATTACK), _animList[STR(Combo::SPECIAL_ATTACK)]);
        }
        if(_currAnimTime > _animList[STR(Combo::SPECIAL_ATTACK)].triggerStartTime) {
            if(!_playedFX) {
                float3 fxPosition = {GetTranslate().x, GetTranslate().y + 6, GetTranslate().z};
                float3 fxRotation = {0, (_model.lock()->GetRotationAxisXYZ().y) * DegToRad, 180 * DegToRad};
                PlayAttackFX(_pEffectList[(int)_componentCombo.lock()->ComboBuff() - 1], fxPosition, fxRotation, _swordSE);
                _playedFX = true;
            }
        }
        break;
    case Combo::SPECIAL_CHARGE:
        AttackAnimation(STR(Combo::SPECIAL_CHARGE), _animList[STR(Combo::SPECIAL_CHARGE)]);
        if(_currAnimTime > _animList[STR(Combo::SPECIAL_CHARGE)].triggerStartTime) {
            if(!_playedFX) {
                float3 fxPosition = {GetTranslate().x, GetTranslate().y + 6, GetTranslate().z};
                float3 fxRotation = {-20 * DegToRad, (_model.lock()->GetRotationAxisXYZ().y) * DegToRad, 180 * DegToRad};
                PlayAttackFX(_pEffectList[(int)_componentCombo.lock()->ComboBuff() - 1], fxPosition, fxRotation, _swordSE);
                _playedFX = true;
            }
        }
        break;
    case Combo::EXSKILL_0: {
        if(_model.lock()->GetPlayAnimationName() != STR(Combo::EXSKILL_0)) {
            _playedFX = false;
            _model.lock()->PlayAnimationNoSame(STR(Combo::EXSKILL_0), false, 0.3f);
            _isPlayingEXSkill = true;
            _isInvincible     = true;
        }

        // 必殺技演出
        _exSkillTimer -= GetDeltaTime60();
        _exSkillTimer          = std::max(0.0f, _exSkillTimer);
        float  t               = abs(1 - (_exSkillTimer / EXSKILL_PLAY_TIME));
        float  yawRad          = (_model.lock()->GetRotationAxisXYZ().y) * DegToRad;
        float  distanceInFront = -5.0f;
        float  offsetX         = sinf(yawRad) * distanceInFront;
        float  offsetZ         = cosf(yawRad) * distanceInFront;
        float  offsetY         = 8.0f;
        float3 targetCamPos    = {GetTranslate().x + offsetX, GetTranslate().y + offsetY, GetTranslate().z + offsetZ};
        // 補間でアクションカメラの位置から正面に移る
        float3 newCamPos       = SlerpWithCenter(GetTranslate(), _camera.lock()->GetTranslate(), targetCamPos, t);
        _exSkillCamera.lock()->SetCurrentCamera();
        _exSkillCamera.lock()->SetPositionAndTarget(newCamPos, GetTranslate() + EXSKILL_CAMERA_TARGET_OFFSET);
        if(!_model.lock()->IsPlaying()) {
            _currCombo        = Combo::EXSKILL_1;
            _isPlayingEXSkill = false;
            _camera.lock()->SetCurrentCamera();
            _exSkillTimer = EXSKILL_PLAY_TIME;
        }
        if(!_playedFX) {
            float3 fxRotation = {0, (_model.lock()->GetRotationAxisXYZ().y) * DegToRad, 0};
            PlayAttackFX(_exSkillEffect, GetTranslate(), fxRotation, _exSkill0SE);
            _playingEffect = SetSpeedPlayingEffekseer3DEffect(_playingEffect, 0.5f);
            _playedFX      = true;
        }
        break;
    }
    case Combo::EXSKILL_1:
        AttackAnimation(STR(Combo::EXSKILL_1), _animList[STR(Combo::EXSKILL_1)], Combo::NO_COMBO, true);
        _playingEffect = SetSpeedPlayingEffekseer3DEffect(_playingEffect, 2.0f);
        if(!_playedFX) {
            PlaySoundMem(_exSkill1SE, DX_PLAYTYPE_BACK);
            ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), _exSkill1SE);
            _playedFX = true;
        }
        if(_model.lock()->GetPlayAnimationName() != STR(Combo::EXSKILL_1)) {
            _isInvincible = false;
            _componentCombo.lock()->ClearCombo();
        }
        break;
    default:
        _playerState = PlayerState::IDLE;
        _currCombo   = Combo::NO_COMBO;
        break;
    }
}

void Player::AttackAnimation(std::string animName, AnimInfo animInfo, Combo nextCombo, bool isEXSkill) {
    if(_model.lock()->GetPlayAnimationName() != animName) {
        _currAnimName = animName;
        this->SetModelRotation();
        _model.lock()->PlayAnimationNoSame(animName, false, 0.2f, _animList[animName].animStartTime);
        _model.lock()->SetAnimationSpeed(animInfo.animStartSpeed);
        _attackList.clear();
        _playedFX = false;
    }
    _currAnimTime = _model.lock()->GetAnimationPlayTime();

    // 指定のフレームに着いたら攻撃判定が有効にする
    if(_currAnimTime > _animList[animName].triggerStartTime) {
        // 敵に当たったらヒットストップが発生する
        if(_isHit) {
            _model.lock()->SetAnimationSpeed(animInfo.animSpeed * 0);
        } else if(_slowMotion) {
            _model.lock()->SetAnimationSpeed(animInfo.animSpeed * 0.01f);
        } else {
            _model.lock()->SetAnimationSpeed(animInfo.animSpeed);
        }
        if(isEXSkill) {
            _exSkillCollision.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY |
                                                           (u32)ComponentCollision::CollisionGroup::ITEM);
            _exSkillCollision.lock()->Overlap((u32)ComponentCollision::CollisionGroup::ENEMY |
                                              (u32)ComponentCollision::CollisionGroup::ITEM);

        } else {
            _weaponCollision.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY |
                                                          (u32)ComponentCollision::CollisionGroup::ITEM);
        }
    }

    // 指定のフレームに着いたら攻撃判定が無効にする
    if(_currAnimTime > _animList[animName].triggerEndTime) {
        _weaponCollision.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
        _exSkillCollision.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
        if(isEXSkill) {
            _isInvincible = false;
            _componentCombo.lock()->ClearCombo();
            _exSkillCollision.lock()->Overlap((u32)ComponentCollision::CollisionGroup::ENEMY);
        }
    }

    // 指定のフレームに着いたら入力次第アニメションが中断する
    if(_currAnimTime > _animList[animName].animCutInTime) {
        _currCombo = Combo::NO_COMBO;
        if(_isCombo) {
            _currCombo = nextCombo;
            _isCombo   = false;
        }
    }
}

void Player::PlayAttackFX(int effect, float3 position, float3 rotation, int soundEffect) {
    _playingEffect = PlayEffekseer3DEffect(effect);
    SetPosPlayingEffekseer3DEffect(_playingEffect, position.x, position.y, position.z);
    SetRotationPlayingEffekseer3DEffect(_playingEffect, rotation.x, rotation.y, rotation.z);
    if(soundEffect != -1) {
        PlaySoundMem(soundEffect, DX_PLAYTYPE_BACK);
        ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), soundEffect);
    }
}

void Player::Die() {
    _model.lock()->PlayAnimationNoSame(STR(PlayerState::DEAD));
}

bool Player::IsDead() {
    return _componentHP.lock()->GetHP() <= 0;
}

void Player::PlayDead() {
    _playerState = PlayerState::DEAD;
}

void Player::SlowMotion() {
    _slowMotion = true;
    _model.lock()->SetAnimationSpeed(_model.lock()->GetAnimationSpeed() * 0.0001f);
}

void Player::EndSlowMotion() {
    _slowMotion = false;
}

bool Player::IsPlayingEXSkill() {
    return _isPlayingEXSkill;
}

void Player::SetModelRotation() {
    if(IsFloat3Zero(_movement)) {
        return;
    }

    float x     = -_movement.x;
    float z     = -_movement.z;
    float theta = atan2(x, z) * RadToDeg;
    _model.lock()->SetRotationAxisXYZ({0, theta, 0});
}

void Player::SetSceneState(Scene::SceneState state) {
    _sceneState  = state;
    _playerState = PlayerState::IDLE;
}

// アニメションのキーオーフレームと速度を設定する
void Player::SetAnimInfo() {
    AnimInfo info         = {};
    info.animStartTime    = 0;
    info.triggerStartTime = 56;
    info.triggerEndTime   = 66;
    info.animCutInTime    = 82;
    info.animStartSpeed   = 3.5f;
    info.animSpeed        = 3.5f;

    _animList[STR(Combo::NORMAL_COMBO1)] = info;

    info                  = {};
    info.animStartTime    = 8;
    info.triggerStartTime = 20;
    info.triggerEndTime   = 35;
    info.animCutInTime    = 43;
    info.animStartSpeed   = 3.5f;
    info.animSpeed        = 3.5f;

    _animList[STR(Combo::NORMAL_COMBO2)] = info;

    info                  = {};
    info.animStartTime    = 33;
    info.triggerStartTime = 45;
    info.triggerEndTime   = 58;
    info.animCutInTime    = 82;
    info.animStartSpeed   = 3.0f;
    info.animSpeed        = 3.0f;

    _animList[STR(Combo::NORMAL_COMBO3)] = info;

    info                  = {};
    info.animStartTime    = 0;
    info.triggerStartTime = 55;
    info.triggerEndTime   = 68;
    info.animCutInTime    = 88;
    info.animStartSpeed   = 3.0f;
    info.animSpeed        = 3.0f;

    _animList[STR(Combo::NORMAL_COMBO4)] = info;

    info                  = {};
    info.triggerStartTime = 55;
    info.triggerEndTime   = 72;
    info.animCutInTime    = 75;
    info.animStartSpeed   = 3.0f;
    info.animSpeed        = 3.0f;

    _animList[STR(Combo::SPECIAL_ATTACK)] = info;

    info                  = {};
    info.animStartTime    = 12;
    info.triggerStartTime = 55;
    info.triggerEndTime   = 65;
    info.animCutInTime    = 80;
    info.animStartSpeed   = 3.0f;
    info.animSpeed        = 3.0f;

    _animList[STR(Combo::SPECIAL_CHARGE)] = info;

    info                             = {};
    info.animStartTime               = 20;
    info.triggerStartTime            = 70;
    info.triggerEndTime              = 170;
    info.animCutInTime               = 190;
    info.animStartSpeed              = 3.0f;
    info.animSpeed                   = 1.5f;
    _animList[STR(Combo::EXSKILL_1)] = info;
}

// コンボゲージの溜まる値を攻撃ごとに設定する
void Player::SetComboList() {
    _comboList[Combo::NORMAL_COMBO1]  = 1;
    _comboList[Combo::NORMAL_COMBO2]  = 1;
    _comboList[Combo::NORMAL_COMBO3]  = 2;
    _comboList[Combo::NORMAL_COMBO4]  = 3;
    _comboList[Combo::SPECIAL_ATTACK] = 1;
    _comboList[Combo::SPECIAL_CHARGE] = 4;
    _comboList[Combo::EXSKILL_1]      = 8;
}

void Player::Exit() {
    delete[] _pEffectList;
    delete[] _pChargeList;

    DeleteSoundMem(_swordSE);
    DeleteSoundMem(_swordHitSE);

    DeleteEffekseerEffect(_slashEffect1);
    DeleteEffekseerEffect(_slashEffect2);
    DeleteEffekseerEffect(_slashEffect3);
    DeleteEffekseerEffect(_hitEffect);
    DeleteEffekseerEffect(_hitBuildingEffect);
    DeleteEffekseerEffect(_exSkillEffect);

    _animList.clear();
    _comboList.clear();
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Player)

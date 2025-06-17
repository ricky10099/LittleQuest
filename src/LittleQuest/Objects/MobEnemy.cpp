#include "MobEnemy.h"
#include "Camera.h"
//#include "LittleQuest/Components/ComponentHP.h"
#include "LittleQuest/Tool.h"
#include "LittleQuest/Objects/Player.h"
#include "LittleQuest/Objects/BreakableObject.h"

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
bool MobEnemy::Init() {
    _initialState = _state = MobEnemyState::IDLE;

    if(_isPatrol) {
        _patrolPoint.push_back(this->GetTranslate() + float3{-50, 0, 0});
        _patrolPoint.push_back(this->GetTranslate() + float3{0, 0, 50});
        _initialState = MobEnemyState::PATROL;
    }

    if(!_patrolPoint.empty()) {
        _patrolIndex = 1;
        _goal        = _patrolPoint[_patrolIndex];
        _state       = MobEnemyState::PATROL;
    }

    _player = Scene::GetObjectPtr<Player>("Player");

    return Super::Init();
}

void MobEnemy::Update() {
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

// 基本描画の後に処理します
void MobEnemy::LateDraw() {
    if(Scene::IsEdit()) {}
    Super::LateDraw();
}

void MobEnemy::Idle() {
    _model.lock()->PlayAnimationNoSame(STR(MobEnemyState::IDLE), true);
}

void MobEnemy::OnHit(const ComponentCollision::HitInfo& hitInfo) {
    if((u32)hitInfo.collision_->GetCollisionGroup() & (u32)ComponentCollision::CollisionGroup::ENEMY_WEAPON) {
        auto* owner = hitInfo.hit_collision_->GetOwner();
        if(auto player = dynamic_cast<Player*>(owner)) {
            if(!_isHitPlayer) {
                _isHitPlayer = true;
                player->GetHit((int)_attackVal);
            }
        }

        if(auto breakableObject = dynamic_cast<BreakableObject*>(owner)) {
            if(!_isHitItem) {
                _isHitItem = true;
                breakableObject->GetHit();
            }
        }
    }

    Super::OnHit(hitInfo);
}

void MobEnemy::SetToSpawnState() {
    ChangeState(MobEnemyState::SPAWN);
}

void MobEnemy::SetSceneState(Scene::SceneState state) {
    _sceneState = state;
    switch(state) {
    case Scene::SceneState::GAME:
        ChangeState(_initialState);
        break;
    }
}

void MobEnemy::SetSpawnPoint(float3 spawnPoint) {
    _spawnPos = spawnPoint;
}

void MobEnemy::SetDetectDistance(float detectDistance) {
    _detectDistance = detectDistance;
}

void MobEnemy::SetDetectAngle(float detectAngle) {
    _detectAngle = detectAngle;
}

float MobEnemy::GetDestroyTimer() const {
    return _destroyTimer;
}

bool MobEnemy::FindPlayer() {
    float distance = GetDistance(_player.lock()->GetTranslate(), this->GetTranslate());
    if(distance < _detectDistance && GetDegreeToPosition(_player.lock()->GetTranslate()) < _detectAngle) {
        ChangeState(MobEnemyState::CHASE);
        return true;
    } else if(_state == MobEnemyState::CHASE && _isPatrol) {
        ChangeState(MobEnemyState::GIVE_UP);
    }
    return false;
}

void MobEnemy::BackToInitialPosition() {
    auto pos    = this->GetTranslate();
    _movement   = _spawnPos - pos;
    _movement.y = 0;
    if(GetDistance(_movement) > 0.5f) {
        _movement = normalize(_movement);

        float x     = -_movement.x;
        float z     = -_movement.z;
        float theta = atan2(x, z) * RadToDeg;

        this->SetRotationAxisXYZ({0, theta, 0});
        _speedFactor = _runFactor;
    } else {
        this->ChangeState(_initialState);
    }
}

void MobEnemy::Patrol() {
    auto pos        = GetTranslate();
    _movement       = _goal - pos;
    _movement.y     = 0;
    float moveValue = GetDistance(_movement);

    if(moveValue <= TRIGGER_POINT_DISTANCE) {
        _patrolIndex++;
        _patrolIndex %= _patrolPoint.size();
        _goal    = _patrolPoint[_patrolIndex];
        _waitFor = PATROL_WAIT_TIME;
        ChangeState(MobEnemyState::WAIT);
        return;
    }

    if(moveValue > TRIGGER_POINT_DISTANCE) {
        _movement = normalize(_movement);

        float x     = -_movement.x;
        float z     = -_movement.z;
        float theta = atan2(x, z) * RadToDeg;
        SetRotationAxisXYZ({0, theta, 0});
        _speedFactor = _walkFactor;
        _model.lock()->PlayAnimationNoSame(STR(MobEnemyState::PATROL), true);
    }
}

void MobEnemy::Wait() {
    _model.lock()->PlayAnimationNoSame(STR(MobEnemyState::IDLE), true, 0.3f);
    _waitFor -= GetDeltaTime60();

    if(_waitFor <= 0.0f) {
        ChangeState(_prevState);
    }
}

void MobEnemy::ChasePlayer() {
    auto pos        = GetTranslate();
    _movement       = _player.lock()->GetTranslate() - pos;
    float moveValue = GetDistance(_movement);

    if(moveValue < ATTACK_DISTANCE) {
        ChangeState(MobEnemyState::ATTACK);
        _detectAngle = ATTACKING_DETECT_ANGLE;
    }

    if(moveValue > 0) {
        _speedFactor = _runFactor;
        _model.lock()->PlayAnimationNoSame(STR(MobEnemyState::CHASE), true);
    }

    _movement   = normalize(_movement);
    float x     = -_movement.x;
    float z     = -_movement.z;
    float theta = atan2(x, z) * RadToDeg;
    SetRotationAxisXYZ({0, theta, 0});
}

void MobEnemy::GetHit(int damage) {
    _componentHP.lock()->TakeDamage(damage);

    if(_componentHP.lock()->GetHP() > 0) {
        _model.lock()->PlayAnimation(STR(MobEnemyState::GET_HIT), false, 0.25f);
        ChangeState(MobEnemyState::GET_HIT);
    } else {
        this->Die();
    }
}

bool MobEnemy::GetIsReady() {
    return _isReady;
}

void MobEnemy::GameAction() {
    if(_isDead) {
        return;
    }

    switch(_state) {
    case MobEnemyState::GET_HIT:
        if(!_model.lock()->IsPlaying()) {
            ChangeState(MobEnemyState::CHASE);
        }
        break;
    case MobEnemyState::GIVE_UP:
        BackToInitialPosition();
        break;
    case MobEnemyState::CHASE:
        FindPlayer();
        ChasePlayer();
        break;
    case MobEnemyState::ATTACK:
        Attack();
        break;
    case MobEnemyState::WAIT:
        Wait();
        break;
    case MobEnemyState::PATROL:
        Patrol();
        FindPlayer();
        break;
    case MobEnemyState::IDLE:
        Idle();
        FindPlayer();
        break;
    }

    _movement *= _baseSpeed * _speedFactor * GetDeltaTime60();
    AddTranslate(_movement);
}

void MobEnemy::TransInAction() {
    switch(_state) {
    case MobEnemyState::SPAWN:
        SpawnAction();
        break;
    default:
        Idle();
        break;
    }
}

void MobEnemy::TransOutAction() {
    this->SetStatus(StatusBit::NoDraw, true);
}

void MobEnemy::ChangeState(MobEnemyState state) {
    this->_prevState = _state;
    this->_state     = state;
}

void MobEnemy::Die() {
    _model.lock()->PlayAnimationNoSame(STR(MobEnemyState::DEAD));
    this->_isDead = true;
    _componentHP.lock()->SetType(ComponentHP::HP_TYPE::NONE);
}

void MobEnemy::SetAnimList() {
    AnimInfo info         = {};
    info.triggerStartTime = 75;
    info.triggerEndTime   = 83;
    info.animCutInTime    = 83;
    info.animSpeed        = 1.2f;
    info.animStartSpeed   = 2.0f;

    _animList[STR(MobEnemyState::ATTACK)] = info;
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::MobEnemy)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::MobEnemy)

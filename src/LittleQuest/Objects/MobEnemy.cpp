#include "MobEnemy.h"
#include "Camera.h"
#include "LittleQuest/Components/ComponentHP.h"
#include "LittleQuest/Tool.h"
#include "LittleQuest/Objects/Player.h"

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
    if(_isPatrol) {
        _patrolPoint.push_back(this->GetTranslate() + float3{-50, 0, 0});
        _patrolPoint.push_back(this->GetTranslate() + float3{0, 0, 50});
    }

    if(!_patrolPoint.empty()) {
        _patrolIndex = 1;
        _goal        = _patrolPoint[_patrolIndex];
        _state       = MobEnemyState::PATROL;
    }

    _initialState = _state = MobEnemyState::IDLE;

    _player = Scene::GetObjectPtr<Player>("Player");

    return Super::Init();
}

void MobEnemy::Update() {
    float deltaTime = GetDeltaTime();

    if(_isDead) {
        _destroyTimer -= deltaTime;
        return;
    }

    if(_state != MobEnemyState::ATTACK && _state != MobEnemyState::GET_HIT) {
        if(!FindPlayer() && _state != MobEnemyState::GIVE_UP && _state != MobEnemyState::WAIT) {
            if(!_patrolPoint.empty()) {
                ChangeState(MobEnemyState::PATROL);
            } else {
                ChangeState(MobEnemyState::IDLE);
            }
        }
    }

    switch(_state) {
    case MobEnemyState::GET_HIT:
        if(!_model.lock()->IsPlaying()) {
            ChangeState(MobEnemyState::IDLE);
        }
        break;
    case MobEnemyState::GIVE_UP:
        BackToInitialPosition();
        break;
    case MobEnemyState::CHASE:
        ChasePlayer();
        break;
    case MobEnemyState::ATTACK:
        Attack();
        break;
    case MobEnemyState::WAIT:
        //Waiting(deltaTime);
        Wait();
        break;
    case MobEnemyState::PATROL:
        Patrol(/*move*/);
        break;
    case MobEnemyState::IDLE:
        Idle();
        break;
    }

    _movement *= _speedBase * _speedFactor * GetDeltaTime60();
    AddTranslate(_movement);
}

// 基本描画の後に処理します
void MobEnemy::LateDraw() {
    if(Scene::IsEdit()) {
        printfDx("\n%s state: %i", this->GetName().data(), _state);
        if(auto modelPtr = GetComponent<ComponentModel>()) {
            printfDx("\n%s %s Animation Time:%f", this->GetName().data(), modelPtr->GetPlayAnimationName().data(),
                     modelPtr->GetAnimationTime());
        }
        printfDx("\ncurpoint: %i", _patrolIndex);
        printfDx("\nx distance: %f", float3(_goal - GetTranslate())[0]);
        printfDx("\nz distance: %f", float3(_goal - GetTranslate())[2]);
        printfDx("\ny distance: %f", float3(_goal - GetTranslate())[1]);
        printfDx("\nf(distance): %f", GetDistance(GetTranslate(), _goal));
        printfDx("\nisFound: %i", _isFoundPlayer);
        printfDx("\ntargetDegree: %f", GetDegreeToPosition(_player.lock()->GetTranslate()));
        printfDx("\ndie timer: %f", _destroyTimer);
    }
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

//void MobEnemy::GUI() {
//    Super::GUI();
//}

//void MobEnemy::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
//    Super::OnHit(hitInfo);
//}

void MobEnemy::Idle() {
    //ChangeState(MobEnemyState::IDLE);
    //if(auto modelPtr = GetComponent<ComponentModel>()) {
    _model.lock()->PlayAnimationNoSame(STR(MobEnemyState::IDLE), true);
    //}
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
    }

    Super::OnHit(hitInfo);
}

void MobEnemy::SetSceneState(Scene::SceneState state) {
    _sceneState = state;
    ChangeState(MobEnemyState::IDLE);
}

void MobEnemy::SetSpawnPoint(float3 spawnPoint) {
    _spawnPos = spawnPoint;
}

float MobEnemy::GetDestroyTimer() {
    return _destroyTimer;
}

bool MobEnemy::FindPlayer() {
    float distance = GetDistance(_player.lock()->GetTranslate(), this->GetTranslate());
    if(distance < 50 && GetDegreeToPosition(_player.lock()->GetTranslate()) < 50) {
        ChangeState(MobEnemyState::CHASE);
        return true;
    } else if(_state == MobEnemyState::CHASE) {
        ChangeState(MobEnemyState::GIVE_UP);
    }
    return false;
}

void MobEnemy::BackToInitialPosition(/*float3& move*/) {
    auto pos    = this->GetTranslate();
    //pos.y    = 0;
    _movement   = _spawnPos - pos;
    _movement.y = 0;
    if(GetDistance(_movement) > 0.5f) {
        _movement = normalize(_movement);

        float x     = -_movement.x;
        float z     = -_movement.z;
        float theta = atan2(x, z) * RadToDeg;

        this->SetRotationAxisXYZ({0, theta, 0});
        _speedFactor = _runVal;
    } else {
        this->ChangeState(_initialState);
    }
}

void MobEnemy::Patrol(/*float3& _movement*/) {
    auto pos        = GetTranslate();
    _movement       = _goal - pos;
    _movement.y     = 0;
    float moveValue = GetDistance(_movement);

    if(moveValue < 5.0f) {
        _patrolIndex++;
        _patrolIndex %= _patrolPoint.size();
        _goal    = _patrolPoint[_patrolIndex];
        _waitFor = _waitTime;
        Wait(/*2.0f*/);
        return;
    }

    if(moveValue > 1.0f) {
        _movement = normalize(_movement);

        float x     = -_movement.x;
        float z     = -_movement.z;
        float theta = atan2(x, z) * RadToDeg;

        SetRotationAxisXYZ({0, theta, 0});
        _speedFactor = _walkVal;
    }
}

void MobEnemy::Wait(/*float time*/) {
    ChangeState(MobEnemyState::WAIT);
    _model.lock()->PlayAnimationNoSame(STR(MobEnemyState::IDLE), true, 0.3f);
    //_waitTime = time;
    _waitFor -= GetDeltaTime60();

    if(_waitFor <= 0.0f) {
        ChangeState(MobEnemyState::IDLE);
    }
}

//void MobEnemy::Waiting(float deltaTime) {
//    waitTime -= deltaTime;
//
//    if(waitTime <= 0.0f) {
//        ChangeState(MobEnemyState::IDLE);
//    }
//}

void MobEnemy::ChasePlayer(/*float3& move*/) {
    auto pos  = GetTranslate();
    _movement = _player.lock()->GetTranslate() - pos;

    if(GetDistance(_movement) < 6.0f) {
        ChangeState(MobEnemyState::ATTACK);
        _movement = {0, 0, 0};
        return;
    }

    _model.lock()->PlayAnimationNoSame("Run", true);

    if(GetDistance(_movement) > 0) {
        _movement = normalize(_movement);

        float x     = -_movement.x;
        float z     = -_movement.z;
        float theta = atan2(x, z) * RadToDeg;

        SetRotationAxisXYZ({0, theta, 0});
        _speedFactor = _runVal;
    }
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

void MobEnemy::ChangeState(MobEnemyState state) {
    //_prevState   = this->_state;
    this->_state = state;
}

void MobEnemy::Die() {
    _model.lock()->PlayAnimationNoSame(STR(MobEnemyState::DEAD));
    RemoveComponent<ComponentCollisionCapsule>();
    this->_isDead = true;
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

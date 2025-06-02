#pragma once

//#include "player.h"
#include <LittleQuest/Objects/Enemy.h>
#include <System/Scene.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
USING_PTR(MobEnemy);

class ComponentHP;
class MobEnemy: public Enemy {
   public:
    BP_OBJECT_DECL(MobEnemy, "LittleQuest/MobEnemy");
    //static MobEnemyPtr Create(const float3& pos, bool isPatrol = true);

    virtual bool Init() override;
    virtual void Update() override;
    virtual void Idle() override;
    virtual void GetHit(int damage) override;
    float        GetDestroyTimer();

   protected:
    enum class MobEnemyState {
        IDLE,
        PATROL,
        GIVE_UP,
        WAIT,
        CHASING,
        ATTACK,
        GET_HIT,
        DEAD,
    };
    MobEnemyState _initialState = MobEnemyState::IDLE;
    MobEnemyState _state        = _initialState;
    MobEnemyState _prevState    = _initialState;
    void          ChangeState(MobEnemyState state);

    virtual void BackToInitialPosition(float3& move);
    virtual void Patrol(float3& move);

    //virtual void Wait(/*float time*/);
    //virtual void Waiting(float deltaTime);

    virtual bool FindPlayer();
    virtual void ChasePlayer(/*float3& move*/) override;
    virtual void Wait() override;
    virtual void Attack() override;
    virtual void Die() override;

    float3              _spawnPos;
    float3              _goal;
    bool                _isPatrol;
    std::vector<float3> _patrolPoint;
    int                 _patrolIndex;
    float               _waitTime;

    bool  _isAttack = false;
    bool  _isHitPlayer;
    bool  _isFoundPlayer;
    float _degree;

    const float _speedBase   = 0.3f;
    const float _walkVal     = 0.5f;
    const float _runVal      = 1.f;
    float       _speedFactor = 1.0f;

    bool  _isDead       = false;
    float _destroyTimer = 5;

    //ObjectWeakPtr                 pPlayer;
    //std::weak_ptr<ComponentModel> pModel;
    //std::weak_ptr<ComponentHP>    pHP;
};
}    // namespace LittleQuest

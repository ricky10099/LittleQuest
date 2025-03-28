﻿#pragma once

#include "player.h"

#include <System/Scene.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
USING_PTR(Enemy);

class ComponentHP;
class Enemy: public Object {
   public:
    BP_OBJECT_DECL(Enemy, "LittleQuest/Enemy");
    static EnemyPtr Create(const float3& pos, bool isPatrol = true, bool isBoss = false);

    bool Init() override;
    void Update() override;
    void LateDraw() override;
    void GUI() override;
    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;

    virtual void GetHit(int damage);
    float        getDestroyTimer();

   protected:
    enum class EnemyState : unsigned int {
        IDLE,
        PATROL,
        GIVE_UP,
        WAIT,
        CHASING,
        ATTACK,
        GET_HIT,
        DEAD,
    };
    EnemyState state;
    EnemyState prevState;
    EnemyState initialState;
    void       ChangeState(EnemyState state);

    virtual void Idle();
    virtual void Die();

    virtual void BackToInitialPosition(float3& move);
    virtual void Patrol(float3& move);

    virtual void Wait(float time);
    virtual void Waiting(float deltaTime);

    virtual bool FindPlayer();
    virtual void ChasePlayer(float3& move);
    virtual void Attack();

    float3              spawnPos;
    float3              goal;
    bool                isPatrol;
    std::vector<float3> patrolPoint;
    int                 patrolIndex;
    float               waitTime;

    bool  isAttack = false;
    bool  isHitPlayer;
    bool  isFoundPlayer;
    float degree;

    const float speedBase   = 0.3f;
    const float walkVal     = 0.5f;
    const float runVal      = 1.f;
    float       speedFactor = 1.0f;

    bool  isDead       = false;
    float destroyTimer = 5;

    ObjectWeakPtr                 pPlayer;
    std::weak_ptr<ComponentModel> pModel;
    std::weak_ptr<ComponentHP>    pHP;
};
}    // namespace LittleQuest

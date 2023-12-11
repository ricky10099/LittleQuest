#pragma once

#include "player.h"

#include <System/Scene.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
USING_PTR(Boss);

class ComponentHP;
class Boss: public Object {
   public:
    BP_OBJECT_TYPE(Boss, Object);
    static BossPtr Create(const float3& pos);

    bool Init() override;
    void Update() override;
    void LateDraw() override;
    void GUI() override;
    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;

    void GetHit(int damage);

   protected:
    const float BASE_SPEED = 0.3f;
    const float WALK_SPEED = 0.5f;
    const float RUN_SPEED  = 1.f;

    enum BossState {
        IDLE,
        WAIT,
        CHASING,
        ATTACK,
        GET_HIT,
        DEAD,
    };
    BossState m_state;

    float3 m_spawnPos;
    float  m_waitTime;
    float  m_degree;
    float  m_speedFactor  = 1.0f;
    float  m_destroyTimer = 5;
    bool   m_isHitPlayer;
    bool   m_isDead = false;

    ObjectWeakPtr                            m_pPlayer;
    std::weak_ptr<ComponentModel>            m_pModel;
    std::weak_ptr<ComponentHP>               m_pHP;
    std::weak_ptr<ComponentCollisionCapsule> m_pLeftHand;
    std::weak_ptr<ComponentCollisionCapsule> m_pRightHand;

    virtual void Idle();
    virtual void Die();
    virtual void Wait(float time);
    virtual void Waiting(float deltaTime);
    virtual void ChasePlayer(float3& move);
    virtual void Attack();
    virtual bool FindPlayer();

    void ChangeState(BossState state);
};
}    // namespace LittleQuest

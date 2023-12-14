#pragma once

#include "player.h"
#include "LittleQuest/Tool.h"

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
    BossState m_prevState;

    enum BossCombo {
        COMBO5,
        NONE,
    };
    BossCombo m_bossCombo;

    enum BossAnim {
        SWIP,
        QUICK_SWIP,
        PUNCH,
        QUICK_PUNCH,
        JUMP_ATTACK,
        BACKFLIP,
        DOUBLE_PUNCH,
        ROAR,
        TAUNT,
    };
    BossAnim m_anim;

    float3 m_spawnPos;
    float  m_waitTime;
    float  m_degree;
    float  m_speedFactor  = 1.0f;
    float  m_destroyTimer = 5;
    int    m_combo;
    bool   m_isHitPlayer;
    bool   m_isDead = false;
    bool   m_combo5 = false;

    ObjectWeakPtr                            m_pPlayer;
    std::weak_ptr<ComponentModel>            m_pModel;
    std::weak_ptr<ComponentHP>               m_pHP;
    std::weak_ptr<ComponentCollisionCapsule> m_pBody;
    std::weak_ptr<ComponentCollisionCapsule> m_pLeftHand;
    std::weak_ptr<ComponentCollisionCapsule> m_pRightHand;
    std::weak_ptr<ComponentCollisionCapsule> m_pAttackCol;

    std::unordered_map<std::string, AnimInfo> m_animList;

    void Idle();
    void Wait(/*float time*/);
    bool FindPlayer();
    void ChasePlayer(float3& move);
    void SelectCombo();
    void Attack();
    void AttackAnimation(std::string animName, AnimInfo animInfo);
    void Combo5();
    void Die();
    void ChangeState(BossState state);
    void SetAnimList();
};
}    // namespace LittleQuest

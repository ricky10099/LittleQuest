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
    void SetSceneState(Scene::SceneState state);
    void PlayTaunt();
    bool IsPlayedTaunt();
    bool IsDead();

   protected:
    const float BASE_SPEED         = 0.3f;
    const float WALK_SPEED         = 0.5f;
    const float RUN_SPEED          = 1.f;
    const float NORMAL_WAIT        = 120.f;
    const float ANGRY_WAIT         = 30.f;
    const float TOO_CLOSE_DISTANCE = 15.0f;
    const float CLOSE_DISTANCE     = 30.0f;
    const float MIDDLE_DISTANCE    = 60.0f;
    const float FAR_DISTANCE       = 90.0;
    const float FRONT_ANGLE        = 60.0f;
    const float BACK_ANGLE         = 140.0f;
    const float DAMAGE_TIME        = 420.0f;
    const int   MAX_HP             = 20;
    const int   DAMAGE_CAP         = MAX_HP * 0.15f;

    enum BossState {
        IDLE,
        WAIT,
        CHASE,
        ATTACK,
        TURN_LEFT,
        TURN_RIGHT,
        GET_HIT,
        ANGRY,
        TAUNT,
        DEAD,
    };
    BossState m_state     = BossState::IDLE;
    BossState m_prevState = BossState::IDLE;

    enum BossCombo {
        SWIP,
        COMBO5,
        FAST_COMBO5,
        BACKFLIP_PUNCH,
        BACKFLIP_CHARGE_PUNCH,
        CHARGE_PUNCH,
        ROAR_ATTACK,

        NONE,
    };
    BossCombo m_bossCombo = BossCombo::NONE;

    enum BossAnim {
        SWIP_ATTACK,
        QUICK_SWIP,
        PUNCH,
        QUICK_PUNCH,
        JUMP_ATTACK,
        BACKFLIP,
        DOUBLE_PUNCH,
        CHARGE,
        ROAR,
        TAUNT_ANIM,
        ANGRY_AURA,
    };
    BossAnim m_anim = BossAnim::TAUNT_ANIM;

    ObjectWeakPtr                             m_pPlayer;
    std::weak_ptr<ComponentModel>             m_pModel;
    std::weak_ptr<ComponentHP>                m_pHP;
    std::weak_ptr<ComponentCollisionCapsule>  m_pBodyBox;
    std::weak_ptr<ComponentCollisionCapsule>  m_pLeftHandBox;
    std::weak_ptr<ComponentCollisionCapsule>  m_pRightHandBox;
    std::weak_ptr<ComponentCollisionSphere>   m_pAngryBox;
    std::unordered_map<std::string, AnimInfo> m_animList;
    //std::weak_ptr<ComponentCollisionCapsule> m_pRoarBox;

    Scene::SceneState m_sceneState = Scene::SceneState::GAME;

    float3 m_move          = {0, 0, 0};
    float  m_waitFor       = 0.0f;
    float  m_waitTime      = NORMAL_WAIT;
    float  m_degree        = 0.0f;
    float  m_speedFactor   = 1.0f;
    float  m_destroyTimer  = 5.0f;
    float  m_currAnimTime  = 0.0f;
    float  m_damageTimer   = 0;
    int    m_combo         = 0;
    int    m_RoarEffect    = -1;
    int    m_powerUpEffect = -1;
    //int    m_angryEffect = -1;
    int    m_playingEffect = -1;
    int    m_RoarPlaying   = -1;
    int    m_damageCount   = 0;
    bool   m_isHitPlayer   = false;
    bool   m_isDead        = false;
    bool   m_isAngry       = false;

    void GameAction();
    void TransInAction();
    void Idle();
    void Wait();
    void ChasePlayer();
    void SelectAction();
    void SelectAngryAction();
    void Attack();
    void AttackAnimation(std::string animName, AnimInfo& animInfo, std::vector<ComponentCollisionCapsulePtr> atkCol = {});
    void Combo5();
    void BackflipPunch();
    void ChargePunch();
    void Swip();
    void Punch();
    //void RoarAttack();
    void PowerUp();
    void Taunt();
    void Damaging();
    void Die();
    void ChangeState(BossState state);
    void SetAnimList();
};
}    // namespace LittleQuest

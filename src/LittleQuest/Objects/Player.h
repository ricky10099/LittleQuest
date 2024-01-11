#pragma once

#include "Enemy.h"
#include "LittleQuest/Tool.h"

#include <System/Scene.h>
#include <vector>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
USING_PTR(Player);

class Camera;
class ComponentHP;
class ComponentCombo;
class Player: public Object {
   public:
    BP_OBJECT_TYPE(Player, Object);
    static PlayerPtr Create(const float3& pos);

    bool Init() override;
    void Update() override;
    void LateDraw() override;
    void GUI() override;
    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;
    void Exit() override;

    void GetHit(int damage);
    void SetSceneState(Scene::SceneState state);
    bool IsDead();
    void PlayDead();

   private:
    const float BASE_SPEED = 0.8f;
    const float RUN_SPEED  = 1.0f;
    const float WALK_SPEED = 0.7f;
    const int   BASE_ATK   = 10;
    const float HIT_PAUSE  = 10;
    const int   MAX_HP     = 100;

    enum PlayerState {
        IDLE,
        WALK,
        RUN,
        ROLL,
        ATTACK,
        GET_HIT,
        DEAD,
    };
    PlayerState m_playerState = PlayerState::IDLE;

    enum Combo {
        NO_COMBO,
        NORMAL_COMBO1,
        NORMAL_COMBO2,
        NORMAL_COMBO3,
        NORMAL_COMBO4,
        SPECIAL_ATTACK,
        SPECIAL_CHARGE,
    };
    Combo currCombo = Combo::NO_COMBO;

    std::unordered_map<std::string, AnimInfo> m_animList;
    std::vector<std::string_view>             m_attackList;
    ObjectWeakPtr                             m_pSword;
    std::weak_ptr<ComponentModel>             m_pModel;
    std::weak_ptr<Camera>                     m_pCamera;
    std::weak_ptr<ComponentHP>                m_pHP;
    std::weak_ptr<ComponentCombo>             m_pCombo;
    std::weak_ptr<ComponentCollisionCapsule>  m_pWeapon;
    //std::unique_ptr<int>                      m_pEffectList;

    Scene::SceneState m_sceneState = Scene::SceneState::GAME;

    matrix m_selfMatrix    = {};
    float3 m_movement      = {0, 0, 0};
    float  m_cameraLength  = 10.0f;
    float  m_speedFactor   = 1.0f;
    float  m_hitTimer      = 0;
    float  m_currAnimSpeed = 1.0f;
    float  m_currAnimTime  = 0;
    bool   m_isCombo       = false;
    bool   m_waitForCombo  = false;
    bool   m_isHit         = false;
    bool   m_isInvincible  = false;
    int    m_hitEffect     = -1;
    int    m_playingEffect = -1;
    int    m_slashEffect1  = -1;
    int    m_slashEffect2  = -1;
    int    m_slashEffect3  = -1;
    int*   m_pEffectList;

    void InputHandle();
    void GameAction();
    void TransOutAction();
    void Idle();
    void Walk();
    void Jump();
    void Attack();
    void Die();
    void AttackAnimation(std::string animName, AnimInfo animInfo, Combo nextCombo = Combo::NO_COMBO);
    void SetModelRotation();
    void SetAnimInfo();
};
}    // namespace LittleQuest

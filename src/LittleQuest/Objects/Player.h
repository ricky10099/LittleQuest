#pragma once

#include "Enemy.h"
#include "Sword.h"
#include "LittleQuest/Tool.h"

#include <System/Scene.h>
#include <vector>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
USING_PTR(Player);

class Camera;
class ComponentHP;
class Player: public Object {
   public:
    BP_OBJECT_TYPE(Player, Object);
    static PlayerPtr Create(const float3& pos, const float3& front = {0, 0, 1});

    bool Init() override;
    void Update() override;
    void LateDraw() override;
    void GUI() override;
    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;
    void GetHit(int damage);

   private:
    void InputHandle();
    void Idle();
    void Walk();
    void Jump();
    void Attack();
    void SetModelRotation();
    void SetAnimInfo();

    std::unordered_map<std::string, AnimInfo> m_animList;
    std::vector<std::string_view>             m_attackList;
    std::weak_ptr<ComponentModel>             m_pModel;
    std::weak_ptr<Camera>                     m_pCamera;
    std::weak_ptr<ComponentHP>                m_pHP;
    std::weak_ptr<ComponentCollisionCapsule>  m_pWeapon;

    matrix m_selfMatrix;
    float3 m_movement;
    float  m_cameraLength;
    bool   m_getHit;

    const float BASE_SPEED = 0.5f;
    const int   BASE_ATK   = 50;

    enum class PlayerState {
        IDLE,
        WALK,
        JUMP,
        ATTACK,
        GET_HIT,
    };
    PlayerState playerState = PlayerState::IDLE;

    enum Combo {
        NO_COMBO,
        NORMAL_COMBO1,
        NORMAL_COMBO2,
        NORMAL_COMBO3,
    };
    Combo currCombo      = Combo::NO_COMBO;
    bool  m_isCombo      = false;
    bool  m_waitForCombo = false;
};
}    // namespace LittleQuest

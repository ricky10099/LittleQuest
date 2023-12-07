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

    std::vector<std::string_view> attackList;
    std::vector<AnimDesc>         AnimList;
    std::weak_ptr<ComponentModel> pModel;
    std::weak_ptr<Camera>         pCamera;
    std::weak_ptr<ComponentHP>    pHP;

    matrix selfMatrix;
    float3 movement;
    float  cameraLength;
    bool   getHit;
    float  speed = 0.5f;

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
    Combo currCombo = Combo::NO_COMBO;

    int  atkVal;
    bool isCombo      = false;
    bool waitForCombo = false;
};
}    // namespace LittleQuest

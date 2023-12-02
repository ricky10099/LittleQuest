#pragma once

#include <System/Scene.h>

#include <vector>

#include "Enemy.h"
#include "Sword.h"

namespace LittleQuest {
    USING_PTR(Player);

    class Player : public Object {
            BP_OBJECT_TYPE(Player, Object);

        public:
            static PlayerPtr Create(const float3& pos, const float3& front = {0, 0, 1});
            bool Init() override;
            void Update() override;
            void LateDraw() override;
            void GUI() override;
            void OnHit(const ComponentCollision::HitInfo& hitInfo) override;
            void GetHit(int damage);

        private:
            enum class PlayerState {
                IDLE,      //!< 待機状態
                WALK,      //!< 歩く
                JUMP,      //!< ジャンプ
                ATTACK,    //!< アタック
                GET_HIT,
            };
            PlayerState playerState = PlayerState::IDLE;

            float speed_ = 0.5f;
            bool isWalk  = false;

            enum Combo {
                COMBO1,
                COMBO2,
                COMBO3,

                NO_COMBO,
            };
            Combo currCombo = Combo::NO_COMBO;

            int atkVal;
            int combo      = 0;
            bool isAttack  = false;
            bool isCombo   = false;
            bool canCombo2 = false;
            bool canCombo3 = false;

            void Idle();
            void Walk(float3& position);
            void Jump();
            void Attack(float3& position);
            void SetModelRotation(float3& position);

            std::vector<std::string_view> attackList;
    };
}    // namespace LittleQuest

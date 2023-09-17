#pragma once

#include "player.h"

#include <System/Scene.h>

namespace LittleQuest {
    USING_PTR(Enemy);

    //! @brief プレイヤー Mouse
    //! @detail Draw()は存在しません。Object標準にて描画されます
    class Enemy : public Object {
        public:
            BP_OBJECT_TYPE(Enemy, Object);

            //! @name システムオーバーライド系
            // @{

            bool Init() override;

            void Update() override;

            // 基本描画の後に処理します
            void LateDraw() override;

            void GUI() override;

            void OnHit(const ComponentCollision::HitInfo& hitInfo) override;

            // @}

            virtual void GetHit(int damage);

            float getDestroyTimer();

        protected:
            virtual void Die();

            virtual void Idle();

            virtual bool isFoundPlayer();

            virtual void Patrol(float3& move);

            virtual void Wait(float time);
            virtual void Waiting(float deltaTime);

            virtual void ChasePlayer(float3& move);
            virtual void Attack();

            virtual void CheckDamageAnimation();

            //! @brief ステート
            enum class EnemyState {
                IDLE,
                PATROL,
                WAIT,
                CHASING,
                ATTACK,
                GET_HIT,
                DEAD,
            };
            EnemyState state;
            EnemyState prevState;
            bool isBusy;

            enum class AnimCheck {
                NONE,
                GETTING_HIT,
                ATTACKING,
            };
            AnimCheck animCheck;

            float3 startPoint;
            float3 endPoint;
            float3 goal;
            std::vector<float3> patrolPoint;
            int patrolIndex;
            float waitTime;

            bool isAttack;
            bool isHitPlayer;

            const float speedBase = 0.3f;
            const float walkVal   = 0.3f;
            const float runVal    = 1.f;
            float speedFactor     = 1.0f;

            float degree;

            float rot_y = 0.0f;

            bool isDie         = false;
            float destroyTimer = 5;
    };
}    // namespace LittleQuest

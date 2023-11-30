#pragma once

#include "player.h"

#include <System/Scene.h>
#include <System/Component/ComponentModel.h>

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

            virtual bool FindPlayer();

            virtual void BackToInitialPosition(float3& move);

            virtual void Patrol(float3& move);

            virtual void Wait(float time);
            virtual void Waiting(float deltaTime);

            virtual void ChasePlayer(float3& move);
            virtual void Attack();

            virtual void CheckAnimation();
            float GetDistance(float3 start, float3 goal) {
                float x = std::pow(goal.x - start.x, 2);
                // float y = std::pow(goal.y - start.y, 2);
                float y = 0;
                float z = std::pow(goal.z - start.z, 2);

                return std::sqrt(x + y + z);
            }
            //! @brief ステート
            enum class EnemyState {
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
            bool isBusy;
            void ChangeState(EnemyState state);

            enum class AnimCheck {
                NONE,
                GETTING_HIT,
                ATTACKING,
            };
            AnimCheck animCheck;

            float3 spawnPos;

            ObjectWeakPtr pPlayer;
            std::weak_ptr<ComponentModel> pModel;

            //巡行関係
            bool isPatrol;
            float3 startPoint;
            float3 endPoint;
            float3 goal;
            std::vector<float3> patrolPoint;
            int patrolIndex;
            float waitTime;

            bool isAttack;
            bool isHitPlayer;
            bool isFoundPlayer;

            const float speedBase = 0.3f;
            const float walkVal   = 0.5f;
            const float runVal    = 1.f;
            float speedFactor     = 1.0f;

            float degree;

            float rot_y = 0.0f;

            bool isDie         = false;
            float destroyTimer = 5;
    };
}    // namespace LittleQuest

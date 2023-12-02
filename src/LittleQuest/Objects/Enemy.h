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

            bool Init() override;

            void Update() override;

            void LateDraw() override;

            void GUI() override;

            void OnHit(const ComponentCollision::HitInfo& hitInfo) override;

            virtual void GetHit(int damage);

            float getDestroyTimer();

        protected:
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

            bool isDie         = false;
            float destroyTimer = 5;
    };
}    // namespace LittleQuest

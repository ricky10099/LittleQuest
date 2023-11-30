#include "Enemy.h"
#include "Camera.h"
#include "../Components/ComponentHP.h"
#include "../src/LittleQuest/Misc.h"

#include <System/Component/Component.h>
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>

namespace LittleQuest {

    //! @brief Editor上でのCreateObject用の設定
    //! @detail BP_OBJECT_TYPEとセットで用意する
    BP_OBJECT_IMPL(Enemy, "LittleQuest/Enemy");

    bool Enemy::Init()    // override
    {
        Super::Init();

        if (isPatrol) {
            // 巡行の座標を設定する
            startPoint = this->GetTranslate() - float3{50, 0, 0};
            patrolPoint.push_back(startPoint);

            endPoint = this->GetTranslate() + float3{50, 0, 0};
            patrolPoint.push_back(endPoint);
        }

        state     = EnemyState::IDLE;
        prevState = state;

        if (!patrolPoint.empty()) {
            patrolIndex = 1;
            goal        = patrolPoint[patrolIndex];
            state       = EnemyState::PATROL;
        }

        initialState = state;

        isAttack = false;
        isBusy   = false;

        pPlayer = Scene::GetObjectPtr<Player>("Player");

        return true;
    }

    void Enemy::Update()    // override
    {
        float deltaTime = GetDeltaTime();

        if (isDie) {
            destroyTimer -= deltaTime;
            return;
        }

        // アニメション中断させないため
        // if (state != EnemyState::GET_HIT && state != EnemyState::ATTACK
        //    && state != EnemyState::WAIT) {
        if (!isBusy) {
            if (FindPlayer()) {
                ChangeState(EnemyState::CHASING);
            } else {
                if (prevState == EnemyState::CHASING) {
                    ChangeState(EnemyState::GIVE_UP);
                } else {
                    if (state != EnemyState::GIVE_UP && state != EnemyState::WAIT) {
                        if (!patrolPoint.empty()) {
                            ChangeState(EnemyState::PATROL);
                        } else {
                            ChangeState(EnemyState::IDLE);
                        }
                    }
                }
            }
        }

        float3 move;

        CheckAnimation();

        switch (state) {
            case EnemyState::GET_HIT:
                break;
            case EnemyState::GIVE_UP:
                BackToInitialPosition(move);
                break;
            case EnemyState::CHASING:
                ChasePlayer(move);
                break;
            case EnemyState::ATTACK:
                Attack();
                break;
            case EnemyState::WAIT:
                Waiting(deltaTime);
                break;
            case EnemyState::PATROL:
                Patrol(move);
                break;
            case EnemyState::IDLE:
                Idle();
                break;
        }

        move *= speedBase * speedFactor * GetDeltaTime60();
        // 地面移動スピードを決定する
        AddTranslate(move);
    }

    // 基本描画の後に処理します
    void Enemy::LateDraw()    // override
    {
        printfDx("\n%s state: %i", this->GetName().data(), state);
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            printfDx("\n%s %s Animation Time:%f", this->GetName().data(), modelPtr->GetPlayAnimationName().data(),
                     modelPtr->GetAnimationTime());
        }
        printfDx("\ncurpoint: %i", patrolIndex);
        printfDx("\nx distance: %f", float3(goal - GetTranslate())[0]);
        printfDx("\nz distance: %f", float3(goal - GetTranslate())[2]);
        printfDx("\nf(distance): %f", GetDistance(GetTranslate(), goal));
        printfDx("\nisFound: %i", isFoundPlayer);
        printfDx("\ntargetDegree: %f", GetDegreeToPosition(pPlayer.lock()->GetTranslate()));
        printfDx("\ndie timer: %f", destroyTimer);
    }

    void Enemy::GUI()    // override
    {
        Super::GUI();

        ImGui::DragFloat(u8"goal座標(T)", &goal[0], 0.01f);
    }

    void Enemy::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo)    // override
    {
        Super::OnHit(hitInfo);
    }

    void Enemy::Idle() {
        // prevState = state;
        // state     = EnemyState::IDLE;
        ChangeState(EnemyState::IDLE);
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            modelPtr->PlayAnimationNoSame("idle", true);
        }
    }

    bool Enemy::FindPlayer() {
        float3 distance = pPlayer.lock()->GetTranslate() - this->GetTranslate();
        distance        = abs(distance);

        if (distance.x < 50 && distance.z < 50) {
            //　プレイヤーが前にいるなら
            if (GetDegreeToPosition(pPlayer.lock()->GetTranslate()) < 50) {
                isFoundPlayer = true;
            }
        } else {
            isFoundPlayer = false;
        }

        return isFoundPlayer;
    }

    void Enemy::BackToInitialPosition(float3& move) {
        auto pos = GetTranslate();
        pos.y    = 0;
        move     = spawnPos - pos;
        if (length(move).x > 0.5) {
            move = normalize(move);

            float x     = -move.x;
            float z     = -move.z;
            float theta = atan2(x, z) * RadToDeg - rot_y;

            SetRotationAxisXYZ({0, theta, 0});
            speedFactor = runVal;
        } else {
            ChangeState(initialState);
        }
    }

    void Enemy::Patrol(float3& move) {
        auto pos        = GetTranslate();
        pos.y           = 0;
        move            = goal - pos;
        float moveValue = GetDistance(goal, pos);

        // 二つの座標を巡行する
        if (moveValue < 5.0f) {
            patrolIndex++;
            patrolIndex %= patrolPoint.size();
            goal = patrolPoint[patrolIndex];

            // ゴール着いたらちょっと待機
            Wait(2.f);
            return;
        }

        if (length(move).x > 1.0f) {
            move = normalize(move);

            float x     = -move.x;
            float z     = -move.z;
            float theta = atan2(x, z) * RadToDeg - rot_y;

            SetRotationAxisXYZ({0, theta, 0});
            speedFactor = walkVal;
        }
    }

    void Enemy::Wait(float time) {
        ChangeState(EnemyState::WAIT);
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            modelPtr->PlayAnimationNoSame("idle", true, 0.3f);
        }
        waitTime = time;
    }

    void Enemy::Waiting(float deltaTime) {
        waitTime -= deltaTime;

        if (waitTime <= 0.0f) {
            ChangeState(EnemyState::IDLE);
            animCheck = AnimCheck::NONE;
            isBusy    = false;
        }
    }

    void Enemy::ChasePlayer(float3& move) {
        auto player = Scene::GetObjectPtr<Player>("Player");
        auto pos    = GetTranslate();
        move        = player->GetTranslate() - pos;

        // この距離から攻撃
        if (abs(move.x) <= float1{7} && abs(move.z) <= float1{7}) {
            ChangeState(EnemyState::ATTACK);
            isBusy = true;
            // 攻撃する時動かないように
            move   = {0, 0, 0};
            return;
        }

        // 走ってプレイヤーに追いつく
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            modelPtr->PlayAnimationNoSame("run", true);
        }
        if (length(move).x > 0) {
            move = normalize(move);

            float x     = -move.x;
            float z     = -move.z;
            float theta = atan2(x, z) * RadToDeg - rot_y;

            SetRotationAxisXYZ({0, theta, 0});
            speedFactor = runVal;
        }
    }

    void Enemy::Attack() {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            modelPtr->PlayAnimationNoSame("attack", false, 0.5f);
            animCheck = AnimCheck::ATTACKING;
        }
    }

    void Enemy::CheckAnimation() {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            switch (animCheck) {
                case AnimCheck::GETTING_HIT:
                    // アニメーション終わったら
                    if (modelPtr->GetAnimationTime() > 0.55f) {
                        state     = EnemyState::IDLE;
                        animCheck = AnimCheck::NONE;
                        isBusy    = false;
                    }
                    break;
                case AnimCheck::ATTACKING:
                    if (modelPtr->GetAnimationTime() > 2.2f) {
                        isAttack    = false;
                        isHitPlayer = false;
                        Wait(.5f);
                    }
                    break;
            }
        }
    }

    void Enemy::GetHit(int damage) {
        auto hpcomponent = GetComponent<ComponentHP>();
        hpcomponent->TakeDamage(damage);

        if (auto modelPtr = GetComponent<ComponentModel>()) {
            if (hpcomponent->GetHP() > 0) {
                modelPtr->PlayAnimation("getHit", false, 0.25f);
                ChangeState(EnemyState::GET_HIT);
                animCheck = AnimCheck::GETTING_HIT;
                isBusy    = true;
            } else {
                this->Die();
            }
        }
    }

    void Enemy::ChangeState(EnemyState state) {
        prevState   = this->state;
        this->state = state;
    }

    void Enemy::Die() {
        if (auto modelPtr = GetComponent<ComponentModel>()) {
            modelPtr->PlayAnimationNoSame("die");
            RemoveComponent<ComponentCollisionCapsule>();
            this->isDie = true;
        }
    }

    float Enemy::getDestroyTimer() {
        return destroyTimer;
    }

}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Enemy)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Enemy)

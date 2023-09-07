#pragma once

#include "player.h"

#include <System/Scene.h>

namespace LittleQuest
{
USING_PTR(Enemy);

//! @brief プレイヤー Mouse
//! @detail Draw()は存在しません。Object標準にて描画されます
class Enemy : public Object
{
public:
    BP_OBJECT_TYPE(Enemy, Object);

    ////! @brief 生成関数
    // static EnemyPtr Create(const float3& pos,
    //                        const float3& front = {0, 0, 1}, const
    //                        std::string name);

    //! @name システムオーバーライド系
    // @{

    bool Init() override;

    void Update() override;

    // 基本描画の後に処理します
    void LateDraw() override;

    void GUI() override;

    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;

    // @}

    virtual void Damaged(int damage);

    float getDieTimer();

protected:
    virtual void Die();

    virtual void Idle();

    virtual void Patrol(float3& move);

    virtual void PatrolWait(float time);
    virtual void PatrolWaiting(float deltaTime);

    virtual void Attack();

    virtual void CheckDamageAnimation();

    void setHP(int HP = 100);

    //! @brief ステート
    enum class EnemyState
    {
        IDLE,   //!< 待機状態
        PATROL,
        WAIT,
        WALK,     //!< 歩く
        RUN,      //!< ジャンプz
        ATTACK,   //!< アタック
        DAMAGED,
    };

    EnemyState state;

    int HP = 100;

    bool isDie = false;

    float3 startPoint;
    float3 endPoint;
    float3 goal;

    std::vector<float3> patrolPoint;
    int                 patrolIndex;

    int animationFrame;

    float waitTime;
    float speed_   = 0.3f;
    float rot_y_   = 0.0f;
    float dieTimer = 300;
};
}   // namespace LittleQuest

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

    virtual void GetHit(int damage);

    float getDieTimer();

protected:
    virtual void Die();

    virtual void Idle();

    virtual bool FindPlayer();

    virtual void Patrol(float3& move);

    virtual void PatrolWait(float time);
    virtual void PatrolWaiting(float deltaTime);

    virtual void Attack(float3& move);

    virtual void CheckDamageAnimation();

    void setHP(int HP = 100);

    //! @brief ステート
    enum class EnemyState
    {
        IDLE,   //!< 待機状態
        PATROL,
        WAIT,
        ATTACK,   //!< アタック
        GET_HIT,
        DEAD,
    };

    EnemyState state;

    int HP = 100;
    int animationFrame;
    int isFoundPlayer;

    float3              startPoint;
    float3              endPoint;
    float3              goal;
    std::vector<float3> patrolPoint;
    int                 patrolIndex;
    float               waitTime;

    const float speedBase   = 0.3f;
    const float walkVal     = 0.3f;
    const float runVal      = 1.f;
    float       speedFactor = 1.0f;

    float degree;

    float rot_y = 0.0f;

    bool  isDie    = false;
    float dieTimer = 300;
};
}   // namespace LittleQuest

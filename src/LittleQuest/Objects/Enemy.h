#pragma once

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
    //static EnemyPtr Create(const float3& pos,
    //                       const float3& front = {0, 0, 1}, const std::string name);

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

    void setHP(int HP = 100);

    //! @brief ステート
    enum class EnemyState
    {
        IDLE,     //!< 待機状態
        WALK,     //!< 歩く
        JUMP,     //!< ジャンプz
        ATTACK,   //!< アタック
        DAMAGED,
    };

    int HP = 100;

    bool isDie = false;

    float dieTimer = 300;
};
}   // namespace LittleQuest

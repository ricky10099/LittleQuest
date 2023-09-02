#pragma once

#include "Enemy.h"
#include <System/Scene.h>

namespace LittleQuest
{
USING_PTR(Mutant);

//! @brief プレイヤー Mouse
//! @detail Draw()は存在しません。Object標準にて描画されます
class Mutant : public Enemy
{
public:
    BP_OBJECT_TYPE(Mutant, Enemy);

    ////! @brief 生成関数
    static MutantPtr Create(const float3& pos, const float3& front = {0, 0, 1});

    //! @name システムオーバーライド系
    // @{

    bool Init() override;

    void Update() override;

    // 基本描画の後に処理します
    void LateDraw() override;

    void GUI() override;

    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;

    // @}

    void Damaged(int damage) override;

protected:
};
}   // namespace LittleQuest

#pragma once

#include "Enemy.h"
#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(Mutant);

//! @brief プレイヤー Mouse
//! @detail Draw()は存在しません。Object標準にて描画されます
class Mutant: public Enemy {
   public:
    BP_OBJECT_TYPE(Mutant, Enemy);

    ////! @brief 生成関数
    static MutantPtr Create(const float3& pos, bool isPatrol = true);

    //! @name システムオーバーライド系
    // @{

    bool Init() override;

    void Update() override;

    // 基本描画の後に処理します
    void LateDraw() override;

    void GUI() override;

    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;

    // @}
    void GetHit(int damage) override;

   protected:
    void Patrol(float3& move) override;

    void BackToInitialPosition(float3& move) override;
    // void Attack() override;

    void Die() override;

   private:
};
}    // namespace LittleQuest

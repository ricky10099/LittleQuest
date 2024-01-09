#pragma once

#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(DestroyedHouse);

//! @brief プレイヤー Mouse
//! @detail Draw()は存在しません。Object標準にて描画されます
class DestroyedHouse: public Object {
   public:
    BP_OBJECT_TYPE(DestroyedHouse, Object);

    //! @brief 生成関数
    static DestroyedHousePtr Create(std::string name = "DestroyedHouse", const float3& pos = {0, 0, 0});

    //! @name システムオーバーライド系
    // @{

    bool Init() override;

    void Update() override;

    // 基本描画の後に処理します
    void LateDraw() override;

    void GUI() override;

    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;

    // @}
};
}    // namespace LittleQuest

#pragma once

#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(DestroyedHouse);
//////////////////////////////////////////////////////////////
//! @brief 破壊された家クラス
//////////////////////////////////////////////////////////////
class DestroyedHouse: public Object {
   public:
    BP_OBJECT_DECL(DestroyedHouse, "LittleQuest/DestroyedHouse");
    static DestroyedHousePtr Create(std::string name = "DestroyedHouse", const float3& pos = {0, 0, 0});

    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    void Update() override;
   private:
    //! モデルコリジョンボックス
    ObjectPtr m_pBox;
};
}    // namespace LittleQuest

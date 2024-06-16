#pragma once

#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(BrokenHouse);
//////////////////////////////////////////////////////////////
//! @brief 壊れた家クラス
//////////////////////////////////////////////////////////////
class BrokenHouse: public Object {
   public:
    BP_OBJECT_DECL(BrokenHouse, "LittleQuest/BrokenHouse");
    static BrokenHousePtr Create(std::string name = "BrokenHouse", const float3& pos = {0, 0, 0});

    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    void Update() override;
   private:
    //! モデルコリジョンボックス
    ObjectPtr m_pBox;
};
}    // namespace LittleQuest

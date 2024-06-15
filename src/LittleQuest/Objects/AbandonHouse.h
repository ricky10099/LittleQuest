#pragma once

#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(AbandonHouse);
//////////////////////////////////////////////////////////////
//! @brief 放棄された家クラス
//////////////////////////////////////////////////////////////
class AbandonHouse: public Object {
   public:
    BP_OBJECT_DECL(AbandonHouse, "LittleQuest/AbandonHouse");
    //------------------------------------------------------------
    //! @brief 放棄された家を生成します。
    //------------------------------------------------------------
    static AbandonHousePtr Create(std::string name = "AbandonHouse", const float3& pos = {0, 0, 0});

    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    void Update() override;
   private:
    //! モデルコリジョンボックス
    ObjectPtr m_pBox;
};
}    // namespace LittleQuest

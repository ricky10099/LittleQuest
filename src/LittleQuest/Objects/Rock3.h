#pragma once

#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(Rock3);
//////////////////////////////////////////////////////////////
//! @brief 石３クラス
//////////////////////////////////////////////////////////////
class Rock3: public Object {
   public:
    BP_OBJECT_DECL(Rock3, "LittleQuest/Rock3");

    //------------------------------------------------------------
    //! @brief 石３を生成します。
    //------------------------------------------------------------
    static Rock3Ptr Create(const float3& pos = {0, 0, 0});

    //------------------------------------------------------------
    //! @brief 初期化処理を行います。
    //!
    //! @retval true 初期化成功
    //! @retval false 初期化失敗
    //------------------------------------------------------------
    bool Init() override;
};
}    // namespace LittleQuest

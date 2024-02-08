#pragma once

#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(Rock1);
//////////////////////////////////////////////////////////////
//! @brief 石１クラス
//////////////////////////////////////////////////////////////
class Rock1: public Object {
   public:
    BP_OBJECT_TYPE(Rock1, Object);
    //------------------------------------------------------------
    //! @brief 石１を生成します。
    //------------------------------------------------------------
    static Rock1Ptr Create(const float3& pos = {0, 0, 0});

    //------------------------------------------------------------
    //! @brief 初期化処理を行います。
    //!
    //! @retval true 初期化成功
    //! @retval false 初期化失敗
    //------------------------------------------------------------
    bool Init() override;
};
}    // namespace LittleQuest

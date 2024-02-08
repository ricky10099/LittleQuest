#pragma once

#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(Fence);
//////////////////////////////////////////////////////////////
//! @brief フェンスクラス
//////////////////////////////////////////////////////////////
class Fence: public Object {
   public:
    BP_OBJECT_TYPE(Fence, Object);
    //------------------------------------------------------------
    //! @brief フェンスを生成します。
    //------------------------------------------------------------
    static FencePtr Create(const float3& pos = {0, 0, 0});

    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    void Update() override;
   private:
    //! モデルコリジョンボックス
    ObjectPtr m_pBox;
};
}    // namespace LittleQuest

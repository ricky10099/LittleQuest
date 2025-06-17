#pragma once

#include <System/Scene.h>
#include <LittleQuest/Objects/BreakableObject.h>

namespace LittleQuest {
USING_PTR(DestroyedHouse);
//////////////////////////////////////////////////////////////
//! @brief 破壊された家クラス
//////////////////////////////////////////////////////////////
class DestroyedHouse: public BreakableObject {
   public:
    BP_OBJECT_DECL(DestroyedHouse, "LittleQuest/DestroyedHouse");
    //------------------------------------------------------------
    //! @brief 破壊された家を生成します。
    //------------------------------------------------------------
    static DestroyedHousePtr Create(std::string name = "DestroyedHouse", const float3& pos = {0, 0, 0});

    virtual bool Init() override;
    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    virtual void Update() override;

   private:
    std::weak_ptr<ComponentModel> _collisionModel;
};
}    // namespace LittleQuest

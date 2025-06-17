#pragma once

#include <System/Scene.h>
#include <LittleQuest/Objects/BreakableObject.h>

namespace LittleQuest {
USING_PTR(BrokenHouse);
//////////////////////////////////////////////////////////////
//! @brief 壊れた家クラス
//////////////////////////////////////////////////////////////
class BrokenHouse: public BreakableObject {
   public:
    BP_OBJECT_DECL(BrokenHouse, "LittleQuest/BrokenHouse");
    static BrokenHousePtr Create(std::string name = "BrokenHouse", const float3& pos = {0, 0, 0});

    virtual bool Init() override;
    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    virtual void Update() override;
   protected:
    std::weak_ptr<ComponentModel> _collisionModel;
};
}    // namespace LittleQuest

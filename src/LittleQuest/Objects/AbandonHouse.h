#pragma once

#include <System/Scene.h>
#include <LittleQuest/Objects/BreakableObject.h>

namespace LittleQuest {
USING_PTR(AbandonHouse);
//////////////////////////////////////////////////////////////
//! @brief 放棄された家クラス
//////////////////////////////////////////////////////////////
class AbandonHouse: public BreakableObject {
   public:
    BP_OBJECT_DECL(AbandonHouse, "LittleQuest/AbandonHouse");
    //------------------------------------------------------------
    //! @brief 放棄された家を生成します。
    //------------------------------------------------------------
    static AbandonHousePtr Create(std::string name = "AbandonHouse", const float3& pos = {0, 0, 0});

    virtual bool Init() override;
    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    virtual void Update() override;

   private:
    std::weak_ptr<ComponentModel> _collisionModel;
};
}    // namespace LittleQuest

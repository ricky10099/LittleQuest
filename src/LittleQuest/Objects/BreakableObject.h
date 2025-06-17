#pragma once

#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(BreakableObject);

class ComponentHP;
//////////////////////////////////////////////////////////////
//! @brief 破壊できるオブジェクトクラス
//////////////////////////////////////////////////////////////
class BreakableObject: public Object {
   public:
    BP_OBJECT_DECL(BreakableObject, "LittleQuest/BreakableObject");

    virtual bool Init() override;
    virtual void GetHit(int damage = 1);

   protected:
    const int DEFAULT_MAX_HP = 5;
    const int MAX_VOLUME     = 255;

    int  _maxHP         = DEFAULT_MAX_HP;
    bool _isBroke       = false;
    int  _breakEffect   = -1;
    int  _playingEffect = -1;
    int  _damageSE      = -1;
    int  _destroySE     = -1;

    std::weak_ptr<ComponentCollisionModel> _collisionBox;
    std::weak_ptr<ComponentModel>          _model;
    std::weak_ptr<ComponentHP>             _componentHP;

    virtual void Break();
};
}    // namespace LittleQuest

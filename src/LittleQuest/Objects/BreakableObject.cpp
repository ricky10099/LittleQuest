#include "BreakableObject.h"

#include <LittleQuest/Components/ComponentHP.h>

namespace LittleQuest {
bool BreakableObject::Init() {
    _componentHP = AddComponent<ComponentHP>();
    _componentHP.lock()->SetType(ComponentHP::HP_TYPE::NONE);
    _componentHP.lock()->SetHP(_maxHP);

    _breakEffect = LoadEffekseerEffect("data/LittleQuest/Effect/ObjectBreak.efk", 5.0f);

    return Super::Init();
}
void BreakableObject::GetHit() {
    if(!_isBroke) {
        _componentHP.lock()->TakeDamage(1);
    }

    if(_componentHP.lock()->GetHP() <= 0) {
        Break();
        _isBroke = true;
    }
}
void BreakableObject::Break() {
    _playingEffect = PlayEffekseer3DEffect(_breakEffect);
    SetPosPlayingEffekseer3DEffect(_playingEffect, GetWorldMatrix().translate().x, GetWorldMatrix().translate().y,
                                   GetWorldMatrix().translate().z);
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::BreakableObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::BreakableObject)

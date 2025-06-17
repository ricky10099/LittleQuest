#include "BreakableObject.h"

#include <LittleQuest/Components/ComponentHP.h>

namespace LittleQuest {
bool BreakableObject::Init() {
    _componentHP = AddComponent<ComponentHP>();
    _componentHP.lock()->SetType(ComponentHP::HP_TYPE::NONE);
    _componentHP.lock()->SetHP(_maxHP);

    _breakEffect = LoadEffekseerEffect("data/LittleQuest/Effect/ObjectBreak.efk", 5.0f);
    _damageSE    = LoadSoundMem("data/LittleQuest/Audio/SE/BuildingDamaged.mp3");
    _destroySE   = LoadSoundMem("data/LittleQuest/Audio/SE/BuildingDestroyed.mp3");

    return Super::Init();
}
void BreakableObject::GetHit(int damage) {
    if(!_isBroke) {
        _componentHP.lock()->TakeDamage(damage);
        PlaySoundMem(_damageSE, DX_PLAYTYPE_BACK);
        ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), _damageSE);
    }

    if(_componentHP.lock()->GetHP() <= 0) {
        Break();
        _isBroke = true;
        PlaySoundMem(_destroySE, DX_PLAYTYPE_BACK);
        ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), _destroySE);
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

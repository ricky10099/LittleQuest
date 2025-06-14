#include "BrokenHouse.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
BrokenHousePtr BrokenHouse::Create(std::string name, const float3& pos) {
    auto obj = Scene::CreateObjectPtr<BrokenHouse>();
    obj->SetName(name);
    obj->SetTranslate(pos);
    obj->AddComponent<ComponentModel>("data/LittleQuest/Model/BrokenHouse/BrokenHouse.mv1");

    obj->_collisionBox = Scene::CreateObjectPtr<Object>("BrokenHouseBox");
    obj->_collisionBox->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009_c.mv1");
    obj->_collisionBox->SetTranslate(pos + float3{-3, 0, 0});
    obj->_collisionBox->SetRotationAxisXYZ({0, 90, 0});
    obj->_collisionBox->SetScaleAxisXYZ({1, 1, 16});
    obj->_collisionBox->AddComponent<ComponentCollisionModel>()
        ->SetCollisionGroup(ComponentCollision::CollisionGroup::ITEM)
        ->AttachToModel();

    return obj;
}
void BrokenHouse::Update() {
    if(!_collisionBox->GetStatus(StatusBit::NoDraw)) {
        _collisionBox->SetStatus(StatusBit::NoDraw, true);
    }
}
void BrokenHouse::Break() {
    RemoveAllComponents();
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::BrokenHouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::BrokenHouse)

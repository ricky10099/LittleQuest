#include "DestroyedHouse.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
DestroyedHousePtr DestroyedHouse::Create(std::string name, const float3& pos) {
    auto obj = Scene::CreateObjectPtr<DestroyedHouse>();
    obj->SetName(name);
    obj->SetTranslate(pos);

    return obj;
}

bool DestroyedHouse::Init() {
    _collisionModel = AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009_c.mv1");
    _collisionModel.lock()->SetTranslate(float3{0.9, 0, -2.3});
    _collisionModel.lock()->SetScaleAxisXYZ({0.045, 0.07, 0.5});
    _collisionBox = AddComponent<ComponentCollisionModel>()->SetCollisionGroup(ComponentCollision::CollisionGroup::ITEM);
    _collisionBox.lock()->AttachToModel();
    _model = AddComponent<ComponentModel>("data/LittleQuest/Model/DestroyedHouse/DestroyedHouse.mv1");
    return Super::Init();
}

void DestroyedHouse::Update() {
    if(!_collisionModel.lock()->GetStatus(Component::StatusBit::NoDraw)) {
        _collisionModel.lock()->SetStatus(Component::StatusBit::NoDraw, true);
    }
    if(_isBroke) {
        _model.lock()->SetStatus(Component::StatusBit::NoDraw, true);
        RemoveComponent(_collisionBox.lock());
    }
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::DestroyedHouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::DestroyedHouse)

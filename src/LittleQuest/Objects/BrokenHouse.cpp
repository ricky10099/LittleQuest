#include "BrokenHouse.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
//------------------------------------------------------------
// オブジェクトを生成します。
//------------------------------------------------------------
BrokenHousePtr BrokenHouse::Create(std::string name, const float3& pos) {
    auto obj = Scene::CreateObjectPtr<BrokenHouse>();
    obj->SetName(name);
    obj->SetTranslate(pos);

    return obj;
}
void BrokenHouse::Update() {
    if(!_collisionModel.lock()->GetStatus(Component::StatusBit::NoDraw)) {
        _collisionModel.lock()->SetStatus(Component::StatusBit::NoDraw, true);
    }
    if(_isBroke) {
        _model.lock()->SetStatus(Component::StatusBit::NoDraw, true);
        RemoveComponent(_collisionBox.lock());
    }
}
bool BrokenHouse::Init() {
    _collisionModel = AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009_c.mv1");
    _collisionModel.lock()->SetTranslate(float3{0.7, 0, 0});
    _collisionModel.lock()->SetScaleAxisXYZ({0.045, 0.07, 0.55});
    _collisionBox = AddComponent<ComponentCollisionModel>()->SetCollisionGroup(ComponentCollision::CollisionGroup::ITEM);
    _collisionBox.lock()->AttachToModel();
    _model = AddComponent<ComponentModel>("data/LittleQuest/Model/BrokenHouse/BrokenHouse.mv1");
    return Super::Init();
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::BrokenHouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::BrokenHouse)

#include "AbandonHouse.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
//------------------------------------------------------------
// オブジェクトを生成します。
//------------------------------------------------------------
AbandonHousePtr AbandonHouse::Create(std::string name, const float3& pos) {
    auto obj = Scene::CreateObjectPtr<AbandonHouse>();
    obj->SetName(name);
    obj->SetTranslate(pos);

    return obj;
}

bool AbandonHouse::Init() {
    _collisionModel = AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009_c.mv1");
    _collisionModel.lock()->SetTranslate(float3{1.6, 0, -2});
    _collisionModel.lock()->SetScaleAxisXYZ({0.04, 0.05, 0.7});
    _collisionBox = AddComponent<ComponentCollisionModel>()->SetCollisionGroup(ComponentCollision::CollisionGroup::ITEM);
    _collisionBox.lock()->AttachToModel();
    _model = AddComponent<ComponentModel>("data/LittleQuest/Model/AbandonHouse/AbandonHouse.mv1");
    return Super::Init();
}

//------------------------------------------------------------
// 更新処理を行います。
//------------------------------------------------------------
void AbandonHouse::Update() {
    if(!_collisionModel.lock()->GetStatus(Component::StatusBit::NoDraw)) {
        _collisionModel.lock()->SetStatus(Component::StatusBit::NoDraw, true);
    }
    if(_isBroke) {
        _model.lock()->SetStatus(Component::StatusBit::NoDraw, true);
        RemoveComponent(_collisionBox.lock());
    }
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::AbandonHouse)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::AbandonHouse)

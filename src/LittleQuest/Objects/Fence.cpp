#include "Fence.h"

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
FencePtr Fence::Create(const float3& pos) {
    auto obj = Scene::CreateObjectPtr<Fence>();
    obj->SetName("Fence");
    obj->SetTranslate(pos);
    obj->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009.mv1");

    obj->m_pBox = Scene::CreateObjectPtr<Object>("FenceBox");
    obj->m_pBox->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj009_c.mv1");
    obj->m_pBox->SetTranslate(pos);
    obj->m_pBox->SetRotationAxisXYZ({0, 90, 0});
    obj->m_pBox->AddComponent<ComponentCollisionModel>()->AttachToModel();

    return obj;
}
void Fence::Update() {
    if(!m_pBox->GetStatus(StatusBit::NoDraw)) {
        m_pBox->SetStatus(StatusBit::NoDraw, true);
    }
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::Fence)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::Fence)

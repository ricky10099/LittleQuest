#include "Mutant.h"
#include "Camera.h"
#include "LittleQuest/Components/ComponentHP.h"
#include "LittleQuest/Tool.h"
#include "LittleQuest/Objects/Player.h"

#include <System/Component/Component.h>
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentTargetTracking.h>

namespace LittleQuest {
MutantPtr Mutant::Create(const float3& pos, bool isPatrol) {
    auto pMutatn = Scene::CreateObjectDelayInitialize<Mutant>();
    pMutatn->SetName("Mutant");
    pMutatn->SetMatrix(HelperLib::Math::CreateMatrixByFrontVector({0, 0, 1}));
    pMutatn->SetTranslate(pos);

    return pMutatn;
}

bool Mutant::Init() {
    _model = AddComponent<ComponentModel>("data/LittleQuest/Model/Mutant/Mutant.mv1");
    _model.lock()->SetScaleAxisXYZ({0.05f});
    _model.lock()->SetAnimation({
        {   STR(MobEnemyState::IDLE),    "data/LittleQuest/Anim/MutantSet/MutantIdle.mv1", 0, 1.0f},
        {                      "Run",     "data/LittleQuest/Anim/MutantSet/MutantRun.mv1", 0, 1.0f},
        { STR(MobEnemyState::ATTACK), "data/LittleQuest/Anim/MutantSet/MutantSwiping.mv1", 0, 1.0f},
        {STR(MobEnemyState::GET_HIT),      "data/LittleQuest/Anim/MutantSet/HeavyHit.mv1", 0, 1.0f},
        {   STR(MobEnemyState::DEAD),   "data/LittleQuest/Anim/MutantSet/ZombieDeath.mv1", 0, 1.0f},
    });
    _model.lock()->PlayAnimation(STR(MobEnemyState::IDLE), true);

    _componentHP = AddComponent<ComponentHP>();
    _componentHP.lock()->SetType(ComponentHP::HP_TYPE::MOB);
    _componentHP.lock()->SetHP(MAX_HP);

    return Super::Init();
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::MobEnemy)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::MobEnemy)

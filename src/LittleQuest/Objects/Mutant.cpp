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
    _model.lock()->SetScaleAxisXYZ({0.08f});
    _model.lock()->SetAnimation({
        {   STR(MobEnemyState::IDLE),    "data/LittleQuest/Anim/MutantSet/MutantIdle.mv1", 0, 1.0f},
        {                      "Run",     "data/LittleQuest/Anim/MutantSet/MutantRun.mv1", 0, 1.0f},
        { STR(MobEnemyState::ATTACK), "data/LittleQuest/Anim/MutantSet/MutantSwiping.mv1", 0, 2.0f},
        {STR(MobEnemyState::GET_HIT),      "data/LittleQuest/Anim/MutantSet/HeavyHit.mv1", 0, 1.0f},
        {   STR(MobEnemyState::DEAD),   "data/LittleQuest/Anim/MutantSet/ZombieDeath.mv1", 0, 1.0f},
    });
    _model.lock()->PlayAnimation(STR(MobEnemyState::IDLE), true);

    _componentHP = AddComponent<ComponentHP>();
    _componentHP.lock()->SetType(ComponentHP::HP_TYPE::MOB);
    _componentHP.lock()->SetHP(MAX_HP);

    _bodyBox = AddComponent<ComponentCollisionCapsule>();
    _bodyBox.lock()->SetTranslate({0, 0, -1});
    _bodyBox.lock()->UseGravity();
    _bodyBox.lock()->SetHeight(13);
    _bodyBox.lock()->SetRadius(2.5);
    _bodyBox.lock()->SetMass(100.0f);
    _bodyBox.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);

    _leftHandBox = AddComponent<ComponentCollisionCapsule>();
    _leftHandBox.lock()->AttachToModel("mixamorig:LeftHand");
    _leftHandBox.lock()->SetTranslate({20, -30, 0});
    _leftHandBox.lock()->SetHeight(9.0f);
    _leftHandBox.lock()->SetRadius(1.0f);
    _leftHandBox.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
    _leftHandBox.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
    _leftHandBox.lock()->Overlap(~(u32)ComponentCollision::CollisionGroup::NONE);

    return Super::Init();
}
void Mutant::Attack() {
    AttackAnimation(STR(MobEnemyState::ATTACK), _animList[STR(MobEnemyState::ATTACK)], {_leftHandBox.lock()});
    if(!_model.lock()->IsPlaying()) {
        _isHitPlayer = false;
        _waitFor     = _waitTime;
        Wait(/*.5f*/);
    }
}

void Mutant::AttackAnimation(std::string animName, AnimInfo& animInfo, std::vector<ComponentCollisionCapsulePtr> atkCol,
                             bool playSE) {
    if(_model.lock()->GetPlayAnimationName() != animName) {
        _model.lock()->PlayAnimationNoSame(animName, false, 0.2f, animInfo.animStartTime);
        _model.lock()->SetAnimationSpeed(animInfo.animStartSpeed);
        //_playedSE = false;
    }
    _currAnimTime = _model.lock()->GetAnimationPlayTime();
    if(_currAnimTime >= animInfo.triggerStartTime) {
        if(_slowMotion) {
            _model.lock()->SetAnimationSpeed(animInfo.animSpeed * 0.01f);
        } else {
            _model.lock()->SetAnimationSpeed(animInfo.animSpeed);
        }
        for(int i = 0; i < atkCol.size(); i++) {
            atkCol[i]->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);
        }

        //if(!_playedSE && playSE) {
        //    PlaySoundMem(_attackSE, DX_PLAYTYPE_BACK);
        //    ChangeVolumeSoundMem((int)(MAX_VOLUME * (Scene::GetSEVolume() / 100.0f)), _attackSE);
        //    _playedSE = true;
        //}
    }
    if(_currAnimTime >= animInfo.triggerEndTime) {
        for(int i = 0; i < atkCol.size(); i++) {
            atkCol[i]->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::NONE);
        }
    }
    if(_currAnimTime >= animInfo.animCutInTime) {
        //_combo++;
        _isHitPlayer = false;
    }
}
}    // namespace LittleQuest

CEREAL_REGISTER_TYPE(LittleQuest::MobEnemy)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, LittleQuest::MobEnemy)

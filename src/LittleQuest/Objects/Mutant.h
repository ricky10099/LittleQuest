#pragma once

//#include "player.h"
#include <LittleQuest/Objects/MobEnemy.h>
#include "LittleQuest/Tool.h"

#include <System/Scene.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
USING_PTR(Mutant);

class ComponentHP;
class Mutant: public MobEnemy {
   public:
    BP_OBJECT_DECL(Mutant, "LittleQuest/Mutant");
    static MutantPtr Create(const float3& pos, bool isPatrol = true);

    virtual bool Init() override;
    //virtual void Update() override;
    //virtual void Idle() override;
    //virtual void GetHit(int damage) override;
    //float        GetDestroyTimer();
    //virtual void LateDraw() override;

    inline void SetHideUI(bool isHideUI) {
        _hideUI = isHideUI;
    }

   protected:
    const int MAX_HP  = 50;
    bool      _hideUI = false;

    //! 体のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule> _bodyBox;
    //! 左手のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule> _leftHandBox;
    //virtual void ChangeState(MobEnemyState state) override;

    virtual void SpawnAction() override;

    //virtual void BackToInitialPosition(float3& move) override;
    //virtual void Patrol(float3& move) override;

    //virtual void Wait(/*float time*/) override;
    ////virtual void Waiting(float deltaTime);

    //virtual bool FindPlayer() override;
    //virtual void ChasePlayer(/*float3& move*/) override;
    //virtual void Wait() override;
    virtual void Attack() override;
    void AttackAnimation(std::string animName, AnimInfo& animInfo, std::vector<ComponentCollisionCapsulePtr> atkCol = {},
                         bool playSE = true);
    //------------------------------------------------------------
    //! @brief アニメーションマップを設定します
    //------------------------------------------------------------
    void SetAnimList();
    //virtual void Die() override;

    //float3              _spawnPos;
    //float3              _goal;
    //bool                _isPatrol;
    //std::vector<float3> _patrolPoint;
    //int                 _patrolIndex;
    //float               _waitTime;

    //bool  _isAttack = false;
    //bool  _isHitPlayer;
    //bool  _isFoundPlayer;
    //float _degree;

    //const float _speedBase = 0.3f;
    //const float _walkVal = 0.5f;
    //const float _runVal = 1.f;
    //float       _speedFactor = 1.0f;

    //bool  _isDead = false;
    //float _destroyTimer = 5;
};
}    // namespace LittleQuest

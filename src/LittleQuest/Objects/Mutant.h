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
    static MutantPtr Create(const float3& pos, bool isPatrol = false);

    virtual bool Init() override;

    virtual void SetToSpawnState() override;
    //------------------------------------------------------------
    //! @brief シーンの行動を設定します。
    //------------------------------------------------------------
    virtual void SetSceneState(Scene::SceneState state) override;

   protected:
    // 攻撃間の待つ時間(フレーム)
    const float ATTACK_WAIT_TIME = 60.0f;
    const int   MAX_HP           = 20;
    const int   MAX_VOLUME       = 255;

    int  _spawnEffect   = -1;
    int  _playingEffect = -1;
    int  _attackSE      = -1;
    bool _playedSE      = false;

    //! 体のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule> _bodyBox;
    //! 左手のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule> _leftHandBox;

    virtual void SpawnAction() override;

    virtual void Attack() override;
    void AttackAnimation(std::string animName, AnimInfo& animInfo, std::vector<ComponentCollisionCapsulePtr> atkCol = {},
                         bool playSE = true);
    virtual void Die() override;
    //------------------------------------------------------------
    //! @brief アニメーションマップを設定します
    //------------------------------------------------------------
    void         SetAnimList();
};
}    // namespace LittleQuest

#pragma once

//#include "player.h"
#include <LittleQuest/Objects/Enemy.h>
#include <System/Scene.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
USING_PTR(MobEnemy);

class ComponentHP;
class MobEnemy: public Enemy {
   public:
    BP_OBJECT_DECL(MobEnemy, "LittleQuest/MobEnemy");
    //static MobEnemyPtr Create(const float3& pos, bool isPatrol = true);

    virtual bool Init() override;
    virtual void Update() override;
    virtual void Idle() override;
    virtual void LateDraw() override;
    //------------------------------------------------------------
    //! @brief 当たりのコールバック
    //!
    //! @param hitInfo　当たったコリジョンのヒット情報
    //------------------------------------------------------------
    virtual void OnHit(const ComponentCollision::HitInfo& hitInfo) override;
    //------------------------------------------------------------
    //! @brief シーンの行動を設定します。
    //------------------------------------------------------------
    virtual void Spawn();
    virtual void SetSceneState(Scene::SceneState state) override;
    virtual void SetSpawnPoint(float3 spawnPoint);

    virtual void GetHit(int damage) override;
    float        GetDestroyTimer();

   protected:
    enum class MobEnemyState {
        SPAWN,
        IDLE,
        PATROL,
        GIVE_UP,
        WAIT,
        CHASE,
        ATTACK,
        GET_HIT,
        DEAD,
    };
    MobEnemyState _initialState = MobEnemyState::IDLE;
    MobEnemyState _state        = _initialState;
    //MobEnemyState _prevState = _initialState;

    const float BASE_SPEED  = 0.5f;
    const float WALK_FACTOR = 1.0f;
    const float RUN_FACTOR  = 2.0f;

    bool _hideUI = false;

    float3              _spawnPos;
    float3              _goal;
    bool                _isPatrol;
    std::vector<float3> _patrolPoint;
    int                 _patrolIndex;

    bool  _isAttack = false;
    bool  _isHitPlayer;
    bool  _isFoundPlayer;
    float _degree;
    float _waitTime = 1.0f;

    float _speedFactor = 1.0f;
    float _attackVal   = 20.0f;

    bool  _isDead       = false;
    float _destroyTimer = 5;

    //------------------------------------------------------------
    //! @brief プレイ中の行動
    //------------------------------------------------------------
    virtual void GameAction() override;
    //------------------------------------------------------------
    //! @brief シーンイントロの行動
    //------------------------------------------------------------
    virtual void TransInAction() override;
    //------------------------------------------------------------
    //! @brief シーン終了の行動
    //------------------------------------------------------------
    virtual void TransOutAction() override;

    virtual void ChangeState(MobEnemyState state);

    virtual void BackToInitialPosition(/*float3& move*/);
    virtual void Patrol(/*float3& move*/);

    //virtual void Wait(/*float time*/);
    //virtual void Waiting(float deltaTime);

    virtual void SpawnAction() {};

    virtual bool FindPlayer();
    virtual void ChasePlayer(/*float3& move*/) override;
    virtual void Wait() override;
    virtual void Die() override;

    //------------------------------------------------------------
    //! @brief 攻撃のアニメーション
    //!
    //! @param animName アニメーション名
    //! @param animInfo アニメーション情報
    //! @param atkCol 攻撃を判定するコリション
    //! @param playSE サウンドエフェクトを再生するか
    //------------------------------------------------------------
    /* virtual void AttackAnimation(std::string animName, AnimInfo& animInfo,
                                 std::vector<ComponentCollisionCapsulePtr> atkCol = {}, bool playSE = true) {}*/

    //------------------------------------------------------------
    //! @brief アニメーションマップを設定します
    //------------------------------------------------------------
    void SetAnimList();
};
}    // namespace LittleQuest

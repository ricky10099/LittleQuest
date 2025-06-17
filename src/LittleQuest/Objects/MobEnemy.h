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

    virtual void SetToSpawnState();
    //------------------------------------------------------------
    //! @brief シーンの行動を設定します。
    //------------------------------------------------------------
    virtual void SetSceneState(Scene::SceneState state) override;
    virtual void SetSpawnPoint(float3 spawnPoint);
    virtual void SetDetectDistance(float detectDistance);
    virtual void SetDetectAngle(float detectAngle);
    //virtual void SetHideUI(bool isHide);

    virtual void GetHit(int damage) override;
    virtual bool GetIsReady();
    float        GetDestroyTimer() const;

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
    MobEnemyState _prevState    = _initialState;

    const float PATROL_WAIT_TIME = 60.0f;

    const float TRIGGER_POINT_DISTANCE  = 0.5f;
    const float DEFAULT_DETECT_DISTANCE = 100.0f;
    const float DEFAULT_DETECT_ANGLE    = 90.0f;
    const float ATTACKING_DETECT_ANGLE  = 360.0f;

    const float ATTACK_DISTANCE = 15.0f;

    float _baseSpeed  = 0.2f;
    float _walkFactor = 0.5f;
    float _runFactor  = 2.0f;

    //! 生成する位置
    float3              _spawnPos;
    //! パトロールの次目的地
    float3              _goal;
    //! パトロールするか
    bool                _isPatrol;
    //! パトロールの全部目的地
    std::vector<float3> _patrolPoint;
    int                 _patrolIndex;

    //! プレイヤーを検測する距離
    float _detectDistance = DEFAULT_DETECT_DISTANCE;
    //! プレイヤーを検測する範囲
    float _detectAngle    = DEFAULT_DETECT_ANGLE;

    bool  _isReady  = true;
    bool  _isAttack = false;
    bool  _isHitPlayer;
    bool  _isHitItem;
    bool  _isFoundPlayer;
    float _degree;

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

    virtual void BackToInitialPosition();
    virtual void Patrol();
    virtual void SpawnAction() {};
    virtual bool FindPlayer();
    virtual void ChasePlayer() override;
    virtual void Wait() override;
    virtual void Die() override;
    //------------------------------------------------------------
    //! @brief アニメーションマップを設定します
    //------------------------------------------------------------
    void         SetAnimList();
};
}    // namespace LittleQuest

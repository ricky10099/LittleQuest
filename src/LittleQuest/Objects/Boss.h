#pragma once

#include "player.h"
#include "LittleQuest/Tool.h"

#include <System/Scene.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
USING_PTR(Boss);

class ComponentHP;
//////////////////////////////////////////////////////////////
//! @brief ボスクラス
//////////////////////////////////////////////////////////////
class Boss: public Object {
   public:
    BP_OBJECT_DECL(Boss, "LittleQuest/Boss");
    //------------------------------------------------------------
    //! @brief ボスを生成します。
    //------------------------------------------------------------
    static BossPtr Create(const float3& pos);

    //------------------------------------------------------------
    //! @brief 初期化処理を行います。
    //!
    //! @return 初期化できたかどうか
    //------------------------------------------------------------
    bool Init() override;
    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    void Update() override;
    //------------------------------------------------------------
    //! @brief 遅い描画の処理を行います。
    //------------------------------------------------------------
    void LateDraw() override;
    //------------------------------------------------------------
    //! @brief ImGUIの処理を行います。
    //------------------------------------------------------------
    void GUI() override;
    //------------------------------------------------------------
    //! @brief 当たりのコールバック
    //!
    //! @param hitInfo　当たったコリジョンのヒット情報
    //------------------------------------------------------------
    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;
    //------------------------------------------------------------
    //! @brief 終了処理を行います。
    //------------------------------------------------------------
    void Exit() override;

    //------------------------------------------------------------
    //! @brief 攻撃される処理を行います。
    //!
    //! @param damage 受けるダメージ
    //------------------------------------------------------------
    void GetHit(int damage);
    //------------------------------------------------------------
    //! @brief
    //------------------------------------------------------------
    void SetSceneState(Scene::SceneState state);
    //------------------------------------------------------------
    //! @brief 挑発動画を再生します。
    //------------------------------------------------------------
    void PlayTaunt();
    //------------------------------------------------------------
    //! @brief 死亡動画を再生します。
    //------------------------------------------------------------
    void PlayDead();
    //------------------------------------------------------------
    //! @brief 挑発動画が再生したのか。
    //!
    //! @retval true 再生した
    //! @retval false 再生していない
    //------------------------------------------------------------
    bool IsPlayedTaunt();
    //------------------------------------------------------------
    //! @brief ボスが倒されたのか。
    //!
    //! @retval true 倒された
    //! @retval false 倒されていない
    //------------------------------------------------------------
    bool IsDead();

   protected:
    //! 移動スピード
    const float WALK_SPEED         = 0.5f;
    //! 怒っていない時の待機時間
    const float NORMAL_WAIT        = 120.f;
    //! 怒っている時の待機時間
    const float ANGRY_WAIT         = 30.f;
    //! 超近距離
    const float TOO_CLOSE_DISTANCE = 15.0f;
    //! 近距離
    const float CLOSE_DISTANCE     = 30.0f;
    //! 中距離
    const float MIDDLE_DISTANCE    = 60.0f;
    //! 遠距離
    const float FAR_DISTANCE       = 90.0;
    //! 前の角度
    const float FRONT_ANGLE        = 60.0f;
    //! 背後の角度
    const float BACK_ANGLE         = 140.0f;
    //! ダメージ溜まる時間
    const float DAMAGE_TIME        = 420.0f;
    //! ダメージ溜まるキャップ
    const int   DAMAGE_CAP         = int(MAX_HP * 0.15f);
    //! 最大HP
    const int   MAX_HP             = 500;
    const int   MAX_VOLUME         = 255;

    enum BossState {
        IDLE,
        WAIT,
        CHASE,
        ATTACK,
        TURN_LEFT,
        TURN_RIGHT,
        GET_HIT,
        ANGRY,
        TAUNT,
        DEAD,
    };
    BossState m_state = BossState::IDLE;

    enum BossCombo {
        SWIP,
        COMBO5,
        FAST_COMBO5,
        BACKFLIP_PUNCH,
        BACKFLIP_CHARGE_PUNCH,
        CHARGE_PUNCH,
        ROAR_ATTACK,

        NONE,
    };
    BossCombo m_bossCombo = BossCombo::NONE;

    enum BossAnim {
        SWIP_ATTACK,
        QUICK_SWIP,
        PUNCH,
        QUICK_PUNCH,
        JUMP_ATTACK,
        BACKFLIP,
        DOUBLE_PUNCH,
        CHARGE,
        ROAR,
        TAUNT_ANIM,
        ANGRY_AURA,
    };
    BossAnim m_anim = BossAnim::TAUNT_ANIM;

    ObjectWeakPtr                             m_pPlayer;
    std::weak_ptr<ComponentModel>             m_pModel;
    std::weak_ptr<ComponentHP>                m_pHP;
    std::weak_ptr<ComponentCollisionCapsule>  m_pBodyBox;
    std::weak_ptr<ComponentCollisionCapsule>  m_pLeftHandBox;
    std::weak_ptr<ComponentCollisionCapsule>  m_pRightHandBox;
    std::weak_ptr<ComponentCollisionSphere>   m_pAngryBox;
    std::unordered_map<std::string, AnimInfo> m_animList;
    std::unordered_map<BossCombo, float>      m_comboList;

    Scene::SceneState m_sceneState = Scene::SceneState::GAME;

    float3 m_move                   = {0, 0, 0};
    float  m_waitFor                = 0.0f;
    float  m_waitTime               = NORMAL_WAIT;
    float  m_degree                 = 0.0f;
    float  m_speedFactor            = 1.0f;
    float  m_destroyTimer           = 5.0f;
    float  m_currAnimTime           = 0.0f;
    float  m_damageTimer            = 0;
    int    m_combo                  = 0;
    int    m_powerUpEffect          = -1;
    int    m_punchEffect            = -1;
    int    m_powerPunchEffect       = -1;
    int    m_doublePunchEffect      = -1;
    int    m_powerDoublePunchEffect = -1;
    int    m_playingEffect          = -1;
    int    m_attackSE               = -1;
    int    m_damageCount            = 0;
    int    m_attackVal              = 10;
    int*   m_pEffectList;
    bool   m_isHitPlayer = false;
    bool   m_isDead      = false;
    bool   m_isAngry     = false;
    bool   m_playedSE    = false;

    void GameAction();
    void TransInAction();
    void TransOutAction();
    void Idle();
    void Wait();
    void ChasePlayer();
    void SelectAction();
    void SelectAngryAction();
    void Attack();
    void AttackAnimation(std::string animName, AnimInfo& animInfo, std::vector<ComponentCollisionCapsulePtr> atkCol = {},
                         bool playSE = true);
    void AttackAnimation(std::string animName, AnimInfo& animInfo, bool playSE);
    void Combo5();
    void BackflipPunch();
    void ChargePunch();
    void Swip();
    void PowerUp();
    void Taunt();
    void Damaging();
    void Die();
    void ChangeState(BossState state);
    void SetAnimList();
    void SetComboList();
};
}    // namespace LittleQuest

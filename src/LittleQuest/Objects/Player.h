#pragma once

#include "Enemy.h"
#include "LittleQuest/Tool.h"

#include <System/Scene.h>
#include <vector>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {
USING_PTR(Player);

class Camera;
class ComponentHP;
class ComponentCombo;
//////////////////////////////////////////////////////////////
//! @brief プレイヤークラス
//////////////////////////////////////////////////////////////
class Player: public Object {
   public:
    BP_OBJECT_TYPE(Player, Object);
    //------------------------------------------------------------
    //! @brief プレイヤーを生成します。
    //------------------------------------------------------------
    static PlayerPtr Create(const float3& pos);

    //------------------------------------------------------------
    //! @brief プレイヤーを初期化します。
    //!
    //! @retval true 初期化成功
    //! @retval false 初期化失敗
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
    //! @brief シーンの行動を設定します。
    //------------------------------------------------------------
    void SetSceneState(Scene::SceneState state);
    //------------------------------------------------------------
    //! @brief プレイヤーが倒されたのか。
    //!
    //! @retval true 倒された
    //! @retval false 倒されていない
    //------------------------------------------------------------
    bool IsDead();
    //------------------------------------------------------------
    //! @brief 死亡動画を再生します。
    //------------------------------------------------------------
    void PlayDead();

   private:
    //! 基礎の移動速度
    const float BASE_SPEED          = 40.0f;
    //! 走るの移動倍数
    const float RUN_SPEED           = 1.0f;
    //! 歩きの移動倍数
    const float WALK_SPEED          = 0.5f;
    //! ヒットストップの時間
    const float HIT_PAUSE           = 20;
    //! チャージ必要の時間
    const float SPECIAL_CHARGE_TIME = 75.0f;
    //! 基礎の攻撃力
    const int   BASE_ATK            = 3;
    //! 最大HP
    const int   MAX_HP              = 200;
    //! 最大音量
    const int   MAX_VOLUME          = 255;

    //! プレイヤーの状態の列挙型
    enum PlayerState {
        IDLE,       //! 待機
        WALK,       //! 歩き
        RUN,        //! 走る
        ROLL,       //! 回避
        ATTACK,     //! 攻撃
        GET_HIT,    //! 攻撃された
        DEAD,       //! 倒された
    };
    //! プレイヤー現在の状態
    PlayerState m_playerState = PlayerState::IDLE;

    //! プレイヤーのコンボの列挙型
    enum Combo {
        NO_COMBO,          //! コンボなし
        NORMAL_COMBO1,     //! 普通攻撃１段
        NORMAL_COMBO2,     //! 普通攻撃２段
        NORMAL_COMBO3,     //! 普通攻撃３段
        NORMAL_COMBO4,     //! 普通攻撃４段
        SPECIAL_ATTACK,    //! 特別攻撃
        SPECIAL_CHARGE,    //! チャージ攻撃
    };
    //! 現在のコンボ
    Combo m_currCombo = Combo::NO_COMBO;

    //! アニメーション名とアニメーション情報のマップ
    std::unordered_map<std::string, AnimInfo> m_animList;
    //! 攻撃方法と攻撃力のマップ
    std::unordered_map<Combo, int>            m_comboList;
    //! 攻撃した敵のリスト
    std::vector<std::string_view>             m_attackList;
    //! モデル
    std::weak_ptr<ComponentModel>             m_pModel;
    //! プレイヤーカメラ
    std::weak_ptr<Camera>                     m_pCamera;
    //! HPコンポーネント
    std::weak_ptr<ComponentHP>                m_pHP;
    //! コンボコンポーネント
    std::weak_ptr<ComponentCombo>             m_pCombo;
    //! 武器のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule>  m_pWeapon;

    //! 現在のシーン行動
    Scene::SceneState m_sceneState = Scene::SceneState::GAME;

    //! 自身マトリクス
    matrix m_selfMatrix          = {};
    //! 移動ベクトル
    float3 m_movement            = {0, 0, 0};
    //! カメラ距離
    float  m_cameraLength        = 10.0f;
    //! 移動倍数
    float  m_speedFactor         = 1.0f;
    //! ヒットストップのタイマー
    float  m_hitTimer            = 0.0f;
    //! 現在のアニメーションの速度
    float  m_currAnimSpeed       = 1.0f;
    //! 現在のアニメーションの時間（フレーム）
    float  m_currAnimTime        = 0.0f;
    //! チャージしているタイマー
    float  m_chargeTime          = 0.0f;
    //! コンボ中なのか
    bool   m_isCombo             = false;
    //! 次のコンボ受けるのか
    bool   m_waitForCombo        = false;
    //! 攻撃が当たっているのか
    bool   m_isHit               = false;
    //! 無敵中なのか
    bool   m_isInvincible        = false;
    //! サウンドエフェクトが再生したのか
    bool   m_playedFX            = false;
    //! チャージしていたのか
    bool   m_charged             = false;
    //! 攻撃当たるエフェクト
    int    m_hitEffect           = -1;
    //! 再生しているエフェクト
    int    m_playingEffect       = -1;
    //! 攻撃エフェクト（コンボ１段）
    int    m_slashEffect1        = -1;
    //! 攻撃エフェクト（コンボ２段）
    int    m_slashEffect2        = -1;
    //! 攻撃エフェクト（コンボ３段）
    int    m_slashEffect3        = -1;
    //! チャージしているエフェクト
    int    m_chargingEffect      = -1;
    //! チャージしたエフェクト
    int    m_chargedEffect       = -1;
    //! チャージエフェクト再生ハンドル
    int    m_playingChargeEffect = -1;
    //! 攻撃のサウドエフェクト
    int    m_swordSE             = -1;
    //! 攻撃当たるサウンドエフェクト
    int    m_swordHitSE          = -1;
    //! 攻撃エフェクトリスト
    int*   m_pEffectList;
    //! チャージエフェクトリスト
    int*   m_pChargeList;

    //------------------------------------------------------------
    //! @brief 入力処理を行います。
    //------------------------------------------------------------
    void InputHandle();
    //------------------------------------------------------------
    //! @brief プレイ中の行動。
    //------------------------------------------------------------
    void GameAction();
    //------------------------------------------------------------
    //! @brief シーン終了の行動。
    //------------------------------------------------------------
    void TransOutAction();
    //------------------------------------------------------------
    //! @brief 待機。
    //------------------------------------------------------------
    void Idle();
    //------------------------------------------------------------
    //! @brief 歩く。
    //------------------------------------------------------------
    void Walk();
    //------------------------------------------------------------
    //! @brief 攻撃処理を行います。
    //------------------------------------------------------------
    void Attack();
    //------------------------------------------------------------
    //! @brief 死亡動画を再生します。
    //------------------------------------------------------------
    void Die();
    //------------------------------------------------------------
    //! @brief 攻撃動画を再生します。
    //!
    //! @param animName アニメーション名
    //! @param animInfo アニメーション情報
    //! @param nextCombo 次のコンボ
    //------------------------------------------------------------
    void AttackAnimation(std::string animName, AnimInfo animInfo, Combo nextCombo = Combo::NO_COMBO);
    //------------------------------------------------------------
    //! @brief モデルの回転を設定します。
    //------------------------------------------------------------
    void SetModelRotation();
    //------------------------------------------------------------
    //! @brief アニメーションマップを設定します
    //------------------------------------------------------------
    void SetAnimInfo();
    //------------------------------------------------------------
    //! @brief 死亡動画を再生します。
    //------------------------------------------------------------
    void SetComboList();
};
}    // namespace LittleQuest

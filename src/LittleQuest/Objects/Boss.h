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
    //! 怒っていない時の待つ時間
    const float NORMAL_WAIT        = 120.f;
    //! 怒っている時の待つ時間
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

    //! ボスの状態の列挙型
    enum BossState {
        IDLE,          //! 待機
        WAIT,          //! 待っている
        CHASE,         //! 追いかけ
        ATTACK,        //! 攻撃
        TURN_LEFT,     //! 左回転
        TURN_RIGHT,    //! 右回転
        GET_HIT,       //! 攻撃された
        ANGRY,         //! 怒っている
        TAUNT,         //! 挑発している
        DEAD,          //! 倒れだ
    };
    //! ボス現在の状態
    BossState m_state = BossState::IDLE;

    //! ボスのコンボの列挙型
    enum BossCombo {
        SWIP,              //! 拳を振る
        COMBO5,            //! ５連撃
        BACKFLIP_PUNCH,    //! バク転突進
        CHARGE_PUNCH,      //! チャージ突進
        RANGED_ATTACK,
        CHARGE_EXPLODE,
        BIG_EXPLODE,

        NONE,    //! コンボしていない
    };
    //! ボス現在のコンボ
    BossCombo m_bossCombo = BossCombo::NONE;

    //! ボスの攻撃アニメーションの列挙型
    enum BossAnim {
        SWIP_ATTACK,     //! 右拳を振る
        QUICK_SWIP,      //! 速い右拳を振る
        PUNCH,           //! 左拳を振る
        QUICK_PUNCH,     //! 速い左拳を振る
        BACKFLIP,        //! バク転
        DOUBLE_PUNCH,    //! 両手パンチ
        CHARGE,          //! チャージ
        TAUNT_ANIM,      //! 挑発する
        ANGRY_AURA,      //! 怒り爆発
        RANGED_SHOT,
        EXPLODE_CHARGE,
        EXPLODE,
    };
    //! ボス現在のアニメーション
    BossAnim m_anim = BossAnim::TAUNT_ANIM;

    //! プレイヤー
    ObjectWeakPtr                             m_pPlayer;
    //! モデル
    std::weak_ptr<ComponentModel>             m_pModel;
    //! HPコンポーネント
    std::weak_ptr<ComponentHP>                m_pHP;
    //! 体のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule>  m_pBodyBox;
    //! 左手のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule>  m_pLeftHandBox;
    //! 右手のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule>  m_pRightHandBox;
    //! 怒り爆発のコリションボックス
    std::weak_ptr<ComponentCollisionSphere>   m_pAngryBox;
    //! アニメーション名とアニメーション情報のマップ
    std::unordered_map<std::string, AnimInfo> m_animList;
    //! 攻撃方法と攻撃力のマップ
    std::unordered_map<BossCombo, float>      m_comboList;

    //! 現在のシーン行動
    Scene::SceneState m_sceneState = Scene::SceneState::GAME;

    //! 移動ベクトル
    float3 m_move                   = {0, 0, 0};
    //! 待つ時間タイマー
    float  m_waitFor                = 0.0f;
    //! 待つ時間の長さ
    float  m_waitTime               = NORMAL_WAIT;
    //! プレイヤーとの角度
    float  m_degree                 = 0.0f;
    //! 移動倍数
    float  m_speedFactor            = 1.0f;
    //!
    //float  m_destroyTimer           = 5.0f;
    //! 現在のアニメーションの時間（フレーム）
    float  m_currAnimTime           = 0.0f;
    //! ダメージ溜まるタイマー
    float  m_damageTimer            = 0;
    //! 現在のコンボ数
    int    m_combo                  = 0;
    //! 怒り爆発のエフェクト
    int    m_powerUpEffect          = -1;
    //! パンチのエフェクト
    int    m_punchEffect            = -1;
    //! 怒っているパンチのエフェクト
    int    m_powerPunchEffect       = -1;
    //! 両手パンチのエフェクト
    int    m_doublePunchEffect      = -1;
    //! 怒っている両手パンチのエフェクト
    int    m_powerDoublePunchEffect = -1;
    //! 再生しているエフェクト
    int    m_playingEffect          = -1;
    //! 攻撃のサウンドエフェクト
    int    m_attackSE               = -1;
    //! 溜まっているダメージ
    int    m_damageCount            = 0;
    //! 基礎攻撃力
    int    m_attackVal              = 10;
    //! エフェクトのリスト
    int*   m_pEffectList;
    //! プレイヤー当たったのか
    bool   m_isHitPlayer = false;
    //! 倒されたのか
    bool   m_isDead      = false;
    //! 怒っているのか
    bool   m_isAngry     = false;
    //! サウンドエフェクトが再生しているのか
    bool   m_playedSE    = false;

    bool m_bigExplode = false;

    //------------------------------------------------------------
    //! @brief プレイ中の行動
    //------------------------------------------------------------
    void GameAction();
    //------------------------------------------------------------
    //! @brief シーンイントロの行動
    //------------------------------------------------------------
    void TransInAction();
    //------------------------------------------------------------
    //! @brief シーン終了の行動
    //------------------------------------------------------------
    void TransOutAction();
    //------------------------------------------------------------
    //! @brief 待機
    //------------------------------------------------------------
    void Idle();
    //------------------------------------------------------------
    //! @brief 待つ
    //------------------------------------------------------------
    void Wait();
    //------------------------------------------------------------
    //! @brief プレイヤーを追いかける
    //------------------------------------------------------------
    void ChasePlayer();
    //------------------------------------------------------------
    //! @brief 行動を選択する（一般）
    //------------------------------------------------------------
    void SelectAction();
    //------------------------------------------------------------
    //! @brief 行動を選択する（怒る）
    //------------------------------------------------------------
    void SelectAngryAction();
    //------------------------------------------------------------
    //! @brief 攻撃する
    //------------------------------------------------------------
    void Attack();
    //------------------------------------------------------------
    //! @brief 攻撃のアニメーション
    //!
    //! @param animName アニメーション名
    //! @param animInfo アニメーション情報
    //! @param atkCol 攻撃を判定するコリション
    //! @param playSE サウンドエフェクトを再生するか
    //------------------------------------------------------------
    void AttackAnimation(std::string animName, AnimInfo& animInfo, std::vector<ComponentCollisionCapsulePtr> atkCol = {},
                         bool playSE = true);
    //------------------------------------------------------------
    //! @brief 攻撃のアニメーション
    //!
    //! @param animName アニメーション名
    //! @param animInfo アニメーション情報
    //! @param playSE サウンドエフェクトを再生するか
    //------------------------------------------------------------
    void AttackAnimation(std::string animName, AnimInfo& animInfo, bool playSE);
    //------------------------------------------------------------
    //! @brief ５連撃
    //------------------------------------------------------------
    void Combo5();
    //------------------------------------------------------------
    //! @brief バク転突進
    //------------------------------------------------------------
    void BackflipPunch();
    //------------------------------------------------------------
    //! @brief チャージ突進
    //------------------------------------------------------------
    void ChargePunch();
    //------------------------------------------------------------
    //! @brief 拳を振る
    //------------------------------------------------------------
    void Swip();
    //------------------------------------------------------------
    //! @brief 強化（怒る）
    //------------------------------------------------------------
    void PowerUp();
    //------------------------------------------------------------
    //! @brief 挑発する
    //------------------------------------------------------------
    void Taunt();
    //------------------------------------------------------------
    //! @brief ダメージ受けている
    //------------------------------------------------------------
    void Damaging();
    //------------------------------------------------------------
    //! @brief 倒された
    //------------------------------------------------------------
    void Die();
    //------------------------------------------------------------
    //! @brief 状態変換
    //------------------------------------------------------------
    void ChangeState(BossState state);
    //------------------------------------------------------------
    //! @brief アニメーションマップを設定します
    //------------------------------------------------------------
    void SetAnimList();
    //------------------------------------------------------------
    //! @brief コンボマップを設定します
    //------------------------------------------------------------
    void SetComboList();
};
}    // namespace LittleQuest

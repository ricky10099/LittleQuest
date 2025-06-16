#pragma once

#include "LittleQuest/Tool.h"

#include <System/Scene.h>
#include <System/Component/ComponentModel.h>
#include <LittleQuest/Objects/Enemy.h>

namespace LittleQuest {
USING_PTR(MawJLaygo);

class ComponentHP;
class Player;
//////////////////////////////////////////////////////////////
//! @brief ボスクラス
//////////////////////////////////////////////////////////////
class MawJLaygo: public Enemy {
   public:
    BP_OBJECT_DECL(MawJLaygo, "LittleQuest/MawJLaygo");
    //------------------------------------------------------------
    //! @brief ボスを生成します。
    //------------------------------------------------------------
    static MawJLaygoPtr Create(const float3& pos);

    //------------------------------------------------------------
    //! @brief 初期化処理を行います。
    //!
    //! @retval true 初期化成功
    //! @retval false 初期化失敗
    //------------------------------------------------------------
    virtual bool Init() override;
    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    virtual void Update() override;
    //------------------------------------------------------------
    //! @brief 遅い描画の処理を行います。
    //------------------------------------------------------------
    virtual void LateDraw() override;
    //------------------------------------------------------------
    //! @brief 当たりのコールバック
    //!
    //! @param hitInfo　当たったコリジョンのヒット情報
    //------------------------------------------------------------
    virtual void OnHit(const ComponentCollision::HitInfo& hitInfo) override;
    //------------------------------------------------------------
    //! @brief 終了処理を行います。
    //------------------------------------------------------------
    virtual void Exit() override;

    //------------------------------------------------------------
    //! @brief 攻撃される処理を行います。
    //!
    //! @param damage 受けるダメージ
    //------------------------------------------------------------
    virtual void GetHit(int damage) override;
    //------------------------------------------------------------
    //! @brief シーンの行動を設定します。
    //------------------------------------------------------------
    virtual void SetSceneState(Scene::SceneState state) override;
    //------------------------------------------------------------
    //! @brief 挑発動画を再生します。
    //------------------------------------------------------------
    void         PlayTaunt();
    //------------------------------------------------------------
    //! @brief 死亡動画を再生します。
    //------------------------------------------------------------
    void         PlayDead();
    //------------------------------------------------------------
    //! @brief 挑発動画が再生したのか。
    //!
    //! @retval true 再生した
    //! @retval false 再生していない
    //------------------------------------------------------------
    bool         IsPlayedTaunt();
    //------------------------------------------------------------
    //! @brief ボスが倒されたのか。
    //!
    //! @retval true 倒された
    //! @retval false 倒されていない
    //------------------------------------------------------------
    virtual bool IsDead() override;
    //------------------------------------------------------------
    //! @brief スローモーションを開始します。
    //------------------------------------------------------------
    virtual void SlowMotion() override;
    //------------------------------------------------------------
    //! @brief スローモーションを終了します。
    //------------------------------------------------------------
    virtual void EndSlowMotion() override;

   protected:
    //! 移動スピード
    const float WALK_SPEED         = 0.5f;
    //! 怒っていない時の待つ時間(フレーム)
    const float NORMAL_WAIT        = 120.f;
    //! 怒っている時の待つ時間(フレーム)
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
    //! 最大HP
    const int   MAX_HP             = 200;
    const int   MAX_VOLUME         = 255;

    //! ボスの状態の列挙型
    enum class MawJLaygoState {
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
    MawJLaygoState _state = MawJLaygoState::IDLE;

    //const char* bs[10] = {
    //    "IDLE",          //! 待機
    //    "WAIT",          //! 待っている
    //    "CHASE",         //! 追いかけ
    //    "ATTACK",        //! 攻撃
    //    "TURN_LEFT",     //! 左回転
    //    "TURN_RIGHT",    //! 右回転
    //    "GET_HIT",       //! 攻撃された
    //    "ANGRY",         //! 怒っている
    //    "TAUNT",         //! 挑発している
    //    "DEAD",          //! 倒れだ
    //};

    //! ボスのコンボの列挙型
    enum class MawJLaygoCombo {
        SWIP,              //! 拳を振る
        COMBO5,            //! ５連撃
        BACKFLIP_PUNCH,    //! バク転突進
        CHARGE_PUNCH,      //! チャージ突進
        RANGED_ATTACK,     //! 遠距離攻撃
        CHARGE_EXPLODE,    //! 大爆発チャージ

        NONE,    //! コンボしていない
    };
    //! ボス現在のコンボ
    MawJLaygoCombo _MawJLaygoCombo = MawJLaygoCombo::NONE;

    //! ボスの攻撃アニメーションの列挙型
    enum class MawJLaygoAnim {
        SWIP_ATTACK,       //! 右拳を振る
        QUICK_SWIP,        //! 速い右拳を振る
        PUNCH,             //! 左拳を振る
        QUICK_PUNCH,       //! 速い左拳を振る
        BACKFLIP,          //! バク転
        DOUBLE_PUNCH,      //! 両手パンチ
        CHARGE,            //! チャージ
        TAUNT_ANIM,        //! 挑発する
        ANGRY_AURA,        //! 怒り爆発
        RANGED_SHOT,       //! 遠距離攻撃
        EXPLODE_CHARGE,    //! 大爆発チャージ
        EXPLODE,           //! 大爆発
    };
    //! ボス現在のアニメーション
    MawJLaygoAnim _anim = MawJLaygoAnim::TAUNT_ANIM;

    ////! プレイヤー
    //ObjectWeakPtr                             _player;
    ////! モデル
    //std::weak_ptr<ComponentModel>             _model;
    //! HPコンポーネント
    //std::weak_ptr<ComponentHP>                _componentHP;
    //! 体のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule>  _bodyBox;
    //! 左手のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule>  _leftHandBox;
    //! 右手のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule>  _rightHandBox;
    //! 怒り爆発のコリションボックス
    std::weak_ptr<ComponentCollisionSphere>   _angryBox;
    //! アニメーション名とアニメーション情報のマップ
    std::unordered_map<std::string, AnimInfo> _animList;
    //! 攻撃方法と攻撃力のマップ
    std::unordered_map<MawJLaygoCombo, float> _comboList;

    //! 現在のシーン行動
    Scene::SceneState _sceneState = Scene::SceneState::GAME;

    //! プレイヤーとの角度
    float _degree                 = 0.0f;
    //! ダメージ溜まるタイマー
    float _damageTimer            = 0;
    //! ダメージ溜まるキャップ
    float _damegeCap              = (float)MAX_HP * 0.1f;
    //! 現在のコンボ数
    int   _combo                  = 0;
    //! 怒り爆発のエフェクト
    int   _powerUpEffect          = -1;
    //! パンチのエフェクト
    int   _punchEffect            = -1;
    //! 怒っているパンチのエフェクト
    int   _powerPunchEffect       = -1;
    //! 両手パンチのエフェクト
    int   _doublePunchEffect      = -1;
    //! 怒っている両手パンチのエフェクト
    int   _powerDoublePunchEffect = -1;
    //! 再生しているエフェクト
    int   _playingEffect          = -1;
    //! 攻撃のサウンドエフェクト
    int   _attackSE               = -1;
    //! 溜まっているダメージ
    int   _damageCount            = 0;
    //! 基礎攻撃力
    int   _attackVal              = 10;
    //! エフェクトのリスト
    int*  _pEffectList;
    //! プレイヤー当たったのか
    bool  _isHitPlayer = false;
    //! 倒されたのか
    bool  _isDead      = false;
    //! 怒っているのか
    bool  _isAngry     = false;
    //! サウンドエフェクトが再生しているのか
    bool  _playedSE    = false;

    //bool _bigExplode = false;
    //! スローモーション
    bool _slowMotion = false;

    //bool _hideUI = false;

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
    //------------------------------------------------------------
    //! @brief 待機
    //------------------------------------------------------------
    virtual void Idle() override;
    //------------------------------------------------------------
    //! @brief 待つ
    //------------------------------------------------------------
    virtual void Wait() override;
    //------------------------------------------------------------
    //! @brief プレイヤーを追いかける
    //------------------------------------------------------------
    virtual void ChasePlayer() override;
    //------------------------------------------------------------
    //! @brief 行動を選択する（一般）
    //------------------------------------------------------------
    void         SelectAction();
    //------------------------------------------------------------
    //! @brief 行動を選択する（怒る）
    //------------------------------------------------------------
    void         SelectAngryAction();
    //------------------------------------------------------------
    //! @brief 攻撃する
    //------------------------------------------------------------
    virtual void Attack() override;
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
    //! @brief 遠距離攻撃
    //------------------------------------------------------------
    void RangedShot();
    //------------------------------------------------------------
    //! @brief 大爆発
    //------------------------------------------------------------
    void ChargeExplode();
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
    virtual void Die() override;
    //------------------------------------------------------------
    //! @brief 状態変換
    //------------------------------------------------------------
    void         ChangeState(MawJLaygoState state);
    //------------------------------------------------------------
    //! @brief アニメーションマップを設定します
    //------------------------------------------------------------
    void         SetAnimList();
    //------------------------------------------------------------
    //! @brief コンボマップを設定します
    //------------------------------------------------------------
    void         SetComboList();
};
}    // namespace LittleQuest

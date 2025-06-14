#pragma once

//#include "Boss.h"
#include "Character.h"
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
class Player: public Character {
   public:
    BP_OBJECT_DECL(Player, "LittleQuest/Player");
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

    virtual void ExitHit(const ComponentCollision::HitInfo& hitInfo) override;
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
    virtual void SetSceneState(Scene::SceneState state);
    //------------------------------------------------------------
    //! @brief プレイヤーが倒されたのか。
    //!
    //! @retval true 倒された
    //! @retval false 倒されていない
    //------------------------------------------------------------
    virtual bool IsDead();
    //------------------------------------------------------------
    //! @brief 死亡動画を再生します。
    //------------------------------------------------------------
    virtual void PlayDead();
    //------------------------------------------------------------
    //! @brief スローモーションを行います。
    //------------------------------------------------------------
    virtual void SlowMotion();
    //------------------------------------------------------------
    //! @brief スローモーションを終了します。
    //------------------------------------------------------------
    virtual void EndSlowMotion();

    inline void SetHideUI(bool isHide) {
        _hideUI = isHide;
    }

   private:
    //! 基礎の移動速度
    const float BASE_SPEED          = 40.0f;
    //! 走るときの倍速
    const float RUN_MULTIPLIER      = 2.0f;
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
    PlayerState _playerState = PlayerState::IDLE;

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
    Combo _currCombo = Combo::NO_COMBO;

    //ObjectWeakPtr _boss;

    //! 攻撃方法と攻撃力のマップ
    std::unordered_map<Combo, int>           _comboList;
    //! プレイヤーカメラ
    std::weak_ptr<Camera>                    _camera;
    //! HPコンポーネント
    std::weak_ptr<ComponentHP>               _componentHP;
    //! コンボコンポーネント
    std::weak_ptr<ComponentCombo>            _componentCombo;
    //! 武器のコリションボックス
    std::weak_ptr<ComponentCollisionCapsule> _weaponCollision;
    //! カメラ修正用
    std::weak_ptr<ComponentCollisionLine>    _cameraCorrection;
    ObjectWeakPtr                            _cameraHitObject;
    //std::weak_ptr<ComponentCollisionCapsule>  _pCameraCorrection;

    //! カメラ距離
    float _cameraLength = 10.0f;
    //! チャージしているタイマー
    float _chargeTime   = 0.0f;

    float _blockedDistance = 0.0f;
    //! コンボ中なのか
    bool  _isCombo         = false;
    //! 次のコンボ受けるのか
    bool  _waitForCombo    = false;
    //! 無敵中なのか
    bool  _isInvincible    = false;
    //! サウンドエフェクトが再生したのか
    bool  _playedFX        = false;
    //! チャージしていたのか
    bool  _charged         = false;

    bool _cameraBlocked = false;

    bool _lockOn = false;

    bool _hideUI = false;

    std::string_view _blockedName = "";

    //! 攻撃当たるエフェクト
    int  _hitEffect           = -1;
    //! 再生しているエフェクト
    int  _playingEffect       = -1;
    //! 攻撃エフェクト（コンボ１段）
    int  _slashEffect1        = -1;
    //! 攻撃エフェクト（コンボ２段）
    int  _slashEffect2        = -1;
    //! 攻撃エフェクト（コンボ３段）
    int  _slashEffect3        = -1;
    //! チャージしているエフェクト
    int  _chargingEffect      = -1;
    //! チャージしたエフェクト
    int  _chargedEffect       = -1;
    //! チャージエフェクト再生ハンドル
    int  _playingChargeEffect = -1;
    //! 攻撃のサウドエフェクト
    int  _swordSE             = -1;
    //! 攻撃当たるサウンドエフェクト
    int  _swordHitSE          = -1;
    //! 攻撃エフェクトリスト
    int* _pEffectList;
    //! チャージエフェクトリスト
    int* _pChargeList;

    //------------------------------------------------------------
    //! @brief 入力処理を行います。
    //------------------------------------------------------------
    void         InputHandle();
    //------------------------------------------------------------
    //! @brief カメラロックオンを行います。
    //------------------------------------------------------------
    void         LockOnCamera();
    //------------------------------------------------------------
    //! @brief プレイ中の行動。
    //------------------------------------------------------------
    virtual void GameAction() override;
    //------------------------------------------------------------
    //! @brief シーン終了の行動。
    //------------------------------------------------------------
    virtual void TransOutAction() override;
    //------------------------------------------------------------
    //! @brief 待機。
    //------------------------------------------------------------
    void         Idle();
    //------------------------------------------------------------
    //! @brief 歩く。
    //------------------------------------------------------------
    void         Walk();
    //------------------------------------------------------------
    //! @brief 攻撃処理を行います。
    //------------------------------------------------------------
    void         Attack();
    //------------------------------------------------------------
    //! @brief 死亡動画を再生します。
    //------------------------------------------------------------
    void         Die();
    //------------------------------------------------------------
    //! @brief 攻撃動画を再生します。
    //!
    //! @param animName アニメーション名
    //! @param animInfo アニメーション情報
    //! @param nextCombo 次のコンボ
    //------------------------------------------------------------
    void         AttackAnimation(std::string animName, AnimInfo animInfo, Combo nextCombo = Combo::NO_COMBO);
    //------------------------------------------------------------
    //! @brief モデルの回転を設定します。
    //------------------------------------------------------------
    virtual void SetModelRotation() override;
    //------------------------------------------------------------
    //! @brief アニメーションマップを設定します
    //------------------------------------------------------------
    virtual void SetAnimInfo() override;

    void SetComboList();
};
}    // namespace LittleQuest

#pragma once

#include "LittleQuest/Tool.h"

#include <System/Scene.h>
#include <vector>
#include <System/Component/ComponentModel.h>
#include <LittleQuest/Components/ComponentHP.h>

namespace LittleQuest {
USING_PTR(Character);

//////////////////////////////////////////////////////////////
//! @brief キャラクタークラス
//////////////////////////////////////////////////////////////
class Character: public Object {
   public:
    BP_OBJECT_DECL(Character, "LittleQuest/Character");
    virtual void LateDraw() override {
        if(_componentHP.lock() && !_isHideHP) {
            _componentHP.lock()->DrawHPBar();
        }
    }
    //------------------------------------------------------------
    //! @brief 攻撃される処理を行います。
    //!
    //! @param damage 受けるダメージ
    //------------------------------------------------------------
    virtual void GetHit(int damage) {}
    //------------------------------------------------------------
    //! @brief スローモーションを行います。
    //------------------------------------------------------------
    virtual void SlowMotion() {}
    //------------------------------------------------------------
    //! @brief スローモーションを終了します。
    //------------------------------------------------------------
    virtual void EndSlowMotion() {}
    //------------------------------------------------------------
    //! @brief シーンの行動を設定します。
    //------------------------------------------------------------
    virtual void SetSceneState(Scene::SceneState state) {}

    virtual void SetHideHP(bool isHideHP) {
        _isHideHP = isHideHP;
    }
    //------------------------------------------------------------
    //! @brief プレイヤーが倒されたのか。
    //!
    //! @retval true 倒された
    //! @retval false 倒されていない
    //------------------------------------------------------------
    virtual bool IsDead() {
        return false;
    }

   protected:
    //! 現在のシーン行動
    Scene::SceneState                         _sceneState = Scene::SceneState::GAME;
    //! アニメーション名とアニメーション情報のマップ
    std::unordered_map<std::string, AnimInfo> _animList;
    //! 攻撃した敵のリスト
    std::vector<std::string_view>             _attackList;
    //! モデル
    std::weak_ptr<ComponentModel>             _model;
    //! 体力コンポーネント
    std::weak_ptr<ComponentHP>                _componentHP;
    //! 体力コンポーネントを描画するかどうか
    bool                                      _isHideHP = false;

    //! 自身マトリクス
    matrix _selfMatrix    = {};
    //! 移動ベクトル
    float3 _movement      = {0, 0, 0};
    //! 移動倍数
    float  _speedFactor   = 1.0f;
    //! ヒットストップのタイマー
    float  _hitTimer      = 0.0f;
    //! 現在のアニメーションの速度
    float  _currAnimSpeed = 1.0f;
    //! 現在のアニメーションの時間（フレーム）
    float  _currAnimTime  = 0.0f;

    //! 攻撃が当たっているのか
    bool _isHit = false;

    bool _slowMotion = false;

    std::string _currAnimName = "";

    //------------------------------------------------------------
    //! @brief 待機
    //------------------------------------------------------------
    virtual void Idle() {}
    //------------------------------------------------------------
    //! @brief 倒された
    //------------------------------------------------------------
    virtual void Die() {}
    //------------------------------------------------------------
    //! @brief プレイ中の行動。
    //------------------------------------------------------------
    virtual void GameAction() {}
    //------------------------------------------------------------
    //! @brief シーンイントロの行動
    //------------------------------------------------------------
    virtual void TransInAction() {}
    //------------------------------------------------------------
    //! @brief シーン終了の行動。
    //------------------------------------------------------------
    virtual void TransOutAction() {}
    //------------------------------------------------------------
    //! @brief モデルの回転を設定します。
    //------------------------------------------------------------
    virtual void SetModelRotation() {}
    //------------------------------------------------------------
    //! @brief アニメーションマップを設定します
    //------------------------------------------------------------
    virtual void SetAnimInfo() {}
};
}    // namespace LittleQuest

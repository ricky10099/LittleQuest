#pragma once

#include "LittleQuest/Tool.h"

#include <System/Scene.h>
#include <vector>
#include <System/Component/ComponentModel.h>
#include <LittleQuest/Objects/Character.h>

namespace LittleQuest {
USING_PTR(Enemy);
class ComponentHP;
//////////////////////////////////////////////////////////////
//! @brief エネミークラス
//////////////////////////////////////////////////////////////
class Enemy: public Character {
   public:
    BP_OBJECT_DECL(Enemy, "LittleQuest/Enemy");

    enum class EnemyType {
        Mob,
        Boss,
    };

    virtual bool IsBoss() {
        return _type == EnemyType::Boss;
    }

   protected:
    const float DEFAULT_WAIT_TIME = 120.0f;

    EnemyType                  _type = EnemyType::Mob;
    //! プレイヤー
    ObjectWeakPtr              _player;
    std::weak_ptr<ComponentHP> _componentHP;

    //! 待つ時間タイマー
    float _waitFor  = 0.0f;
    //! 待つ時間の長さ
    float _waitTime = DEFAULT_WAIT_TIME;

    //------------------------------------------------------------
    //! @brief 待つ
    //------------------------------------------------------------
    virtual void Wait() {}
    //------------------------------------------------------------
    //! @brief プレイヤーを追いかける
    //------------------------------------------------------------
    virtual void ChasePlayer() {}
    //------------------------------------------------------------
    //! @brief 攻撃する
    //------------------------------------------------------------
    virtual void Attack() {}
};
}    // namespace LittleQuest

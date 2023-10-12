#pragma once

#include <System/Scene.h>

#include <vector>

#include "Enemy.h"
#include "Sword.h"

namespace LittleQuest {
    USING_PTR(Player);

    //! @brief プレイヤー Mouse
    //! @detail Draw()は存在しません。Object標準にて描画されます
    class Player : public Object {
        public:
            BP_OBJECT_TYPE(Player, Object);

            //! @brief 生成関数
            static PlayerPtr Create(const float3& pos,
                                    const float3& front = {0, 0, 1});

            //! @name システムオーバーライド系
            // @{

            bool Init() override;

            void Update() override;

            // 基本描画の後に処理します
            void LateDraw() override;

            void GUI() override;

            void OnHit(const ComponentCollision::HitInfo& hitInfo) override;

            void GetHit(int damage);
            // @}
            //! @name 内部アクセス用
            // @{

            //! @brief スピードを設定する
            //! @param s スピード
            void SetSpeed(float s);

            //! @brief スピードを取得する
            //! @return 現在のスピード
            float GetSpeed();

            //! @brief 変数として扱う関数
            //! @detail Set/Get系と比べデバックはしにくい
            //! @return 現在のスピードの変数(speed_)
            float& Speed();

            // @}

        private:
            //! @brief プレイヤーステート
            enum class PlayerState {
                IDLE,      //!< 待機状態
                WALK,      //!< 歩く
                JUMP,      //!< ジャンプ
                ATTACK,    //!< アタック
                DAMAGED,
            };
            PlayerState playerState = PlayerState::IDLE;

            float speed_ = 0.5f;
            bool isWalk  = false;
            // float rot_y_   = 0.0f;
            // float rot_x_   = 0.0f;

            int atkVal;
            bool isAttack  = false;
            bool isCombo   = false;
            int combo      = 0;
            bool canCombo2 = false;
            bool canCombo3 = false;

            void Idle();
            void Walk(float3& position);
            void Jump();
            void Attack(float3& position);
            void SetModelRotation(float3& position);

            std::vector<std::string_view> attackList;
    };
}    // namespace LittleQuest

#pragma once

#include <System/Scene.h>

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
                IDLE,       //!< 待機状態
                WALK,       //!< 歩く
                JUMP,       //!< ジャンプ
                ATTACK1,    //!< 地面にタッチしたか
                ATTACK2,    //!< 地面にタッチした!
                ATTACK3,    //!< 終了アクション
                DAMAGED,
            };

            struct Material {
                    std::shared_ptr<Texture> albedo_;
                    std::shared_ptr<Texture> normal_;
                    std::shared_ptr<Texture> roughness_;
                    std::shared_ptr<Texture> metalness_;
            };

            std::vector<Material> materials_;

            PlayerState playerState = PlayerState::IDLE;

            float speed_  = 0.5f;
            float rot_y_  = 0.0f;
            float rot_x_  = 0.0f;
            bool isAttack = false;
            bool isCombo  = false;
            bool isWalk   = false;
            int combo     = 0;
            void Idle();
            void Walk(float3& position);
            void Jump();
            void Attack();
            void Damaged();

            void AttachSword();
    };
}    // namespace LittleQuest

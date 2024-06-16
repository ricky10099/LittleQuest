#include <System/Scene.h>
#include <System/Component/ComponentModel.h>

#include <LittleQuest/Components/ComponentTexture.h>
#include <LittleQuest/Objects/Camera.h>

namespace LittleQuest {
//////////////////////////////////////////////////////////////
//! @brief タイトルシーンクラス
//////////////////////////////////////////////////////////////
class GameTitleScene: public Scene::Base {
   public:
    BP_CLASS_DECL(GameTitleScene, u8"LittleQuest/GameTitleScene");
    //------------------------------------------------------------
    //! @brief シーン名を取得します。
    //!
    //! @return シーン名
    //------------------------------------------------------------
    std::string Name() {
        return "LittleQuest/GameTitleScene";
    }

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
    //! @brief 終了処理を行います。
    //------------------------------------------------------------
    void Exit() override;

   private:
    //! ゲーム始まるまでの時間
    const float  START_TIME       = 120.0f;
    //! カメラの初期位置
    const float3 START_CAM_POS    = {-3, 3, 12};
    //! カメラの初期目標位置
    const float3 START_CAM_TARGET = {0, 1.5f, 0};
    //! カメラの終了位置
    const float3 END_CAM_POS      = {-1, 6, 20};
    //! カメラの終了目標位置
    const float3 END_CAM_TARGET   = {0, 5, 0};

    //! ゲーム始まるまでのタイマー
    float m_startTimer   = START_TIME;
    //! フェイドアウトのアルファ
    float m_alpha        = 0;
    //! シーン経過時間
    float m_elapsed60    = 0;
    //! 文字の幅
    int   m_stringWidth  = 0;
    //! 文字の高さ
    int   m_stringHeight = 0;
    //! エスケープ押した回数
    int   m_escapeCount  = 0;
    //! DXライブラリのフォントハンドル
    int   m_fontHandle   = -1;
    //! 始まるのか
    bool  m_isStart      = false;
    //! 文字を表示するのか
    bool  m_showString   = true;

    //! モデル
    std::weak_ptr<ComponentModel>     m_pModel;
    //! シーンカメラ
    std::weak_ptr<ComponentCamera>    m_pCamera;
    //! タイトル画像
    std::weak_ptr<ComponentTexture2D> m_pTitle;
};

}    // namespace LittleQuest

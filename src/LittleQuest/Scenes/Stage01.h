//---------------------------------------------------------------------------
//! @file   Stage01.h
//! @brief  Stage01シーン
//---------------------------------------------------------------------------
#include <LittleQuest/Objects/Player.h>
#include <LittleQuest/Objects/Boss.h>
#include <LittleQuest/Components/ComponentTexture.h>

#include <vector>
#include <System/Scene.h>
#include <System/Component/ComponentCamera.h>

#pragma once
namespace LittleQuest {
//////////////////////////////////////////////////////////////
//! @brief ステージ１シーンクラス
//////////////////////////////////////////////////////////////
class Stage01: public Scene::Base {
   public:
    BP_CLASS_TYPE(Stage01, Scene::Base)

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

    //! シーンのイントロ中なのか
    bool M_IsCutScene = true;

   private:
    //! シーンのイントロの時間
    const float  START_CUT_SCENE_TIME = 240.0f;
    //! フェイドの時間
    const float  FADE_TIME            = 30.0f;
    //! ゲーム終了画像縮小する時間
    const float  SHRINK_TIME          = 120.0f;
    //! シーンのイントロカメラの初期位置
    const float3 CUT_SCENE_POS_1      = {130, 21, -50};
    //! シーンのイントロカメラの終了位置
    const float3 CUT_SCENE_POS_2      = {-97, 17, -50};
    //! シーンのイントロカメラの初期画角
    const float  FOV_INTRO            = 140.0f;
    //! 標準画角
    const float  FOV_ORG              = 45.0f;
    //! プレイヤー生成位置
    const float3 PLAYER_SPAWN_POS     = {-50, 1, -50};
    //! ボス生成位置
    const float3 BOSS_SPAWN_POS       = {140, 1, -50};
    //! プレイヤー負けたカメラの位置
    const float3 PLAYER_DEATH_CAM     = {-40, 20, -50};
    //! ボス倒されたカメラの位置
    const float3 BOSS_DEATH_CAM       = {115, 20, -50};
    //! 最大音量
    const int    MAX_VOLUME           = 255;
    //! 放棄された家の座標
    const float3 ABANDONHOUSE_POS[8]  = {
         {-205,   2.4,  136},
         {-205, -0.15,   26},
         {-205,  -0.5, -103},
         {-205, 0.165, -197},
         {-110,   1.5,  136},
         {-110,  -0.2,   26},
         {-110,  -0.5, -103},
         {-110,   3.5, -197},
    };
    //! 壊れた家の座標
    const float3 BROKENHOUSE_POS[8] = {
        {205,    7, -179},
        {124,  6.5, -179},
        { 51, 5.5f, -179},
        {-23, 2.5f, -179},
        {205,    8, -259},
        {124,    7, -259},
        { 51, 4.5f, -259},
        {-23, 2.5f, -259},
    };
    //! 破壊された家の座標
    const float3 DESTROYEDHOUSE_POS[8] = {
        {205,    0,  90},
        {124, 0.5f,  90},
        { 51, 0.6f,  90},
        {-23, 2.5f,  90},
        {205,    0, 175},
        {124, 0.5f, 175},
        { 51, 0.6f, 175},
        {-23, 2.5f, 175},
    };

    //! フェイドタイマー
    float m_fadeTimer       = FADE_TIME;
    //! 縮小タイマー
    float m_shrinkTimer     = 0;
    //! シーンのイントロタイマー
    float m_cutSceneTimer   = START_CUT_SCENE_TIME;
    //! フェイドアルファ
    float m_alpha           = 255;
    //! ゲームタイマーの秒数
    float m_second          = 0.0f;
    //! ゲーム経過時間
    float m_elapsed         = 0;
    //! ゲーム終わりタイトルに戻るタイマー
    float m_endingTimer     = 600;
    //! フォントハンドル
    int   m_fontHandle      = -1;
    //! タイマーフォントハンドル
    int   m_timerFontHandle = -1;
    //! イントロのBGM
    int   m_introBGM        = -1;
    //! ゲームのBGM
    int   m_BGM             = -1;
    //! 文字の幅
    int   m_stringWidth     = 0;
    //! 文字の高さ
    int   m_stringHeight    = 0;
    //! ゲームタイマーの幅
    int   m_timerWidth      = 0;
    //! ゲームタイマーの高さ
    int   m_timerHeight     = 0;
    //! ゲームタイマーの分
    int   m_minute          = 3;
    //! 負けるのか
    bool  m_isLose          = false;

    //! プレイヤー
    std::weak_ptr<Player>             m_pPlayer;
    //! ボス
    std::weak_ptr<Boss>               m_pBoss;
    //! プレイヤーカメラ
    std::weak_ptr<Camera>             m_pPlayerCamera;
    //! シーンカメラ
    std::weak_ptr<ComponentCamera>    m_pCamera;
    //! 勝利画像
    std::weak_ptr<ComponentTexture2D> m_pClearImage;
    //! 失敗画像
    std::weak_ptr<ComponentTexture2D> m_pFailImage;
    //! 表示する座標
    std::weak_ptr<ComponentTexture2D> m_pShowImage;

    //------------------------------------------------------------
    //! @brief フェイドインを行います。
    //!
    //! @retval true フェイドイン中
    //! @retval false フェイドインしていない
    //------------------------------------------------------------
    bool FadeIn();
    //------------------------------------------------------------
    //! @brief フェイドアウトを行います。
    //!
    //! @retval true フェイドアウト中
    //! @retval false フェイドアウトしていない
    //------------------------------------------------------------
    bool FadeOut();
    //------------------------------------------------------------
    //! @brief 終了画像を表示します。
    //!
    //! @retval true 終了画像表示完了
    //! @retval false 終了画像表示中
    //------------------------------------------------------------
    bool ShowMessage();
};
}    // namespace LittleQuest

//---------------------------------------------------------------------------
//! @file   Stage01.h
//! @brief  Stage01シーン
//---------------------------------------------------------------------------
#include <LittleQuest/Objects/Player.h>
#include <LittleQuest/Objects/MawJLaygo.h>
#include <LittleQuest/Objects/Mutant.h>
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
    BP_CLASS_DECL(Stage01, u8"LittleQuest/Stage01")

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
    bool IsCutScene = true;

   private:
    //! シーンのイントロの時間
    const float  START_CUT_SCENE_TIME = 240.0f;
    const float  MOB_SPAWN_TIME       = 240.0f;
    //! フェイドの時間
    const float  FADE_TIME            = 30.0f;
    //! ゲーム終了画像縮小する時間
    const float  SHRINK_TIME          = 120.0f;
    //! シーンのイントロカメラの初期位置
    const float3 CUT_SCENE_POS_START  = {100, 21, -50};
    const float3 CUT_SCENE_POS_MOB    = {70, 21, -50};
    const float3 CUT_SCENE_TARGET_MOB = {120, 0, -30};
    //! シーンのイントロカメラの終了位置
    const float3 CUT_SCENE_POS_FINAL  = {-97, 17, -50};
    //! シーンのイントロカメラの初期画角
    //const float  FOV_INTRO            = 140.0f;
    const float  FOV_INTRO            = 45.0f;
    //! 標準画角
    const float  FOV_ORG              = 45.0f;
    //! プレイヤー生成位置
    const float3 PLAYER_SPAWN_POS     = {-50, 1, -50};
    //! ボス生成位置
    const float3 BOSS_SPAWN_POS       = {140, 1, -50};
    const float3 MOB_SPAWN_POS        = {100, 1, -50};
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
    //! 初期雑魚の座標
    const float3 MOB_POS[4] = {
        {100, -18, -30},
        {120, -18, -15},
        {100, -18, -70},
        {120, -18, -85},
    };

    //! フェイドタイマー
    float _fadeTimer       = FADE_TIME;
    //! 縮小タイマー
    float _shrinkTimer     = 0;
    //! 雑魚モンスター出現タイマー
    float _mobSpawnTimer   = MOB_SPAWN_TIME;
    //! シーンのイントロタイマー
    float _cutSceneTimer   = START_CUT_SCENE_TIME;
    //! フェイドアルファ
    float _alpha           = 255;
    //! ゲームタイマーの秒数
    float _second          = 0.0f;
    //! ゲーム経過時間
    float _elapsed         = 0;
    //! ゲーム終わりタイトルに戻るタイマー
    float _endingTimer     = 600;
    //! フォントハンドル
    int   _fontHandle      = -1;
    //! タイマーフォントハンドル
    int   _timerFontHandle = -1;
    //! イントロのBGM
    int   _introBGM        = -1;
    //! ゲームのBGM
    int   _BGM             = -1;
    //! 文字の幅
    int   _stringWidth     = 0;
    //! 文字の高さ
    int   _stringHeight    = 0;
    //! ゲームタイマーの幅
    int   _timerWidth      = 0;
    //! ゲームタイマーの高さ
    int   _timerHeight     = 0;
    //! ゲームタイマーの分
    int   _minute          = 3;
    //! 負けるのか
    bool  _isLose          = false;

    bool _isMobSpawned = false;

    bool _showBlackBar = false;

    bool _slideBlackBar = false;

    //! プレイヤー
    std::weak_ptr<Player>                _pPlayer;
    //! ボス
    std::weak_ptr<MawJLaygo>             _pBoss;
    std::array<std::weak_ptr<Mutant>, 4> _pMob;
    //! プレイヤーカメラ
    std::weak_ptr<Camera>                _pPlayerCamera;
    //! シーンカメラ
    std::weak_ptr<ComponentCamera>       _pCamera;
    //! 勝利画像
    std::weak_ptr<ComponentTexture2D>    _pClearImage;
    //! 失敗画像
    std::weak_ptr<ComponentTexture2D>    _pFailImage;
    //! 表示する座標
    std::weak_ptr<ComponentTexture2D>    _pShowImage;

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

    void ShowBlackBar();
    //------------------------------------------------------------
    //! @brief 終了画像を表示します。
    //!
    //! @retval true 終了画像表示完了
    //! @retval false 終了画像表示中
    //------------------------------------------------------------
    bool ShowMessage();
};
}    // namespace LittleQuest

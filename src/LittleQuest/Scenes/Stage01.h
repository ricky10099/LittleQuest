//---------------------------------------------------------------------------
//! @file   Stage01.h
//! @brief  Stage01シーン
//---------------------------------------------------------------------------
#pragma once
#include <LittleQuest/Objects/Player.h>
#include <LittleQuest/Objects/Boss.h>

#include <vector>
#include <System/Scene.h>
#include <System/Component/ComponentCamera.h>

namespace LittleQuest {
//class ComponentCamera;
//===========================================================================
//! Stage01シーン
//===========================================================================
class Stage01: public Scene::Base {
   public:
    BP_CLASS_TYPE(Stage01, Scene::Base)

    bool Init() override;      //!< 初期化
    void Update() override;    //!< 更新
    void Draw() override;
    void LateDraw() override;    //!< 描画
    void Exit() override;        //!< 終了
    void GUI() override;         //!< GUI表示

    bool M_IsCutScene = true;

   private:
    //std::vector<EnemyPtr> enemies;
    const float  START_CUT_SCENE_TIME = 240.0f;
    const float  FADE_TIME            = 30.0f;
    const float  SHRINK_TIME          = 120.0f;
    const float3 CUT_SCENE_POS_1      = {130, 21, -50};
    const float3 CUT_SCENE_POS_2      = {-97, 17, -50};
    const float  FOV_INTRO            = 140.0f;
    const float  FOV_ORG              = 45.0f;
    const float3 PLAYER_SPAWN_POS     = {-50, 1, -50};
    const float3 BOSS_SPAWN_POS       = {140, 1, -50};
    const float3 PLAYER_DEATH_CAM     = {-40, 20, -50};
    const float3 BOSS_DEATH_CAM       = {115, 20, -50};

    float m_fadeTimer     = FADE_TIME;
    float m_shrinkTimer   = 0;
    float m_cutSceneTimer = START_CUT_SCENE_TIME;
    float m_alpha         = 255;
    int   m_clearImage    = -1;
    int   m_failImage     = -1;
    int   m_showImage     = -1;
    int   m_fontHandle    = -1;
    int   m_introBGM      = -1;
    int   m_BGM           = -1;
    int   m_loseAudio     = -1;
    int   m_stringWidth   = 0;
    int   m_stringHeight  = 0;
    bool  m_isLoading     = true;

    std::weak_ptr<Player>          m_pPlayer;
    std::weak_ptr<Boss>            m_pBoss;
    std::weak_ptr<Camera>          m_pPlayerCamera;
    std::weak_ptr<ComponentCamera> m_pCamera;

    bool FadeIn();
    bool FadeOut();
    bool ShowMessage();
};
}    // namespace LittleQuest

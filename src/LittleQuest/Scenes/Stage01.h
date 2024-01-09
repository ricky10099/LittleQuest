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

    bool Init() override;        //!< 初期化
    void Update() override;      //!< 更新
    void LateDraw() override;    //!< 描画
    void Exit() override;        //!< 終了
    void GUI() override;         //!< GUI表示

    bool M_IsCutScene = true;

   private:
    //std::vector<EnemyPtr> enemies;
    const float  START_CUT_SCENE_TIME = 240;
    const float  FADE_TIME            = 60;
    const float3 CUT_SCENE_POS_1      = {130, 20, -50};
    const float3 CUT_SCENE_POS_2      = {-97, 17, -50};
    const float  FOV_1                = 140.0f;
    const float  ORG_FOV              = 45;

    float m_fadeTimer     = FADE_TIME;
    float m_cutSceneTimer = START_CUT_SCENE_TIME;
    float m_alpha         = 255;
    bool  m_isLoading     = true;

    std::weak_ptr<Player>          m_pPlayer;
    std::weak_ptr<Boss>            m_pBoss;
    std::weak_ptr<Camera>          m_pPlayerCamera;
    std::weak_ptr<ComponentCamera> m_pCamera;
};
}    // namespace LittleQuest

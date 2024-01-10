//---------------------------------------------------------------------------
//! @file   Stage01.cpp
//! @brief  Stage01シーン
//---------------------------------------------------------------------------
#include "Stage01.h"

#include <LittleQuest/Objects/Camera.h>
#include <LittleQuest/Objects/AbandonHouse.h>
#include <LittleQuest/Objects/BrokenHouse.h>
#include <LittleQuest/Objects/DestroyedHouse.h>
//#include <LittleQuest/Objects/Enemy.h>
//#include <LittleQuest/Objects/Zombie.h>
//#include <LittleQuest/Objects/Mutant.h>
//#include <LittleQuest/Objects/Player.h>
//#include <LittleQuest/Objects/Boss.h>
#include <LittleQuest/Scenes/GameOverScene.h>
#include <LittleQuest/Scenes/GameWinScene.h>
#include <LittleQuest/Scenes/GameTitleScene.h>

#include <System/Component/ComponentAttachModel.h>
//#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

namespace LittleQuest {

BP_CLASS_IMPL(Stage01, u8"LittleQuest/Stage01")

//===========================================================================
//  Stage01シーン
//===========================================================================

//---------------------------------------------------------------------------
//! 初期化
//---------------------------------------------------------------------------
bool Stage01::Init() {
    if(AddFontResourceEx("data/LittleQuest/Fonts/MPLUSCodeLatin-Regular.ttf", FR_PRIVATE, NULL) > 0) {
    } else {
        MessageBox(NULL, "フォント読込失敗", "", MB_OK);
    }
    m_fontHandle = CreateFontToHandle("M PLUS Code Latin", 30, 4, DX_FONTTYPE_ANTIALIASING_EDGE, DX_CHARSET_UTF8, 1);
    GetDrawStringSizeToHandle(&m_stringWidth, &m_stringHeight, NULL, "Press any key back to Title", -1, m_fontHandle);
    m_clearImage = LoadGraph("data/LittleQuest/Image/clear.png");
    m_failImage  = LoadGraph("data/LittleQuest/Image/failure.png");

    scene_state = Scene::SceneState::TRANS_IN;

    // Environment
    {
        auto groundObj = Scene::CreateObjectPtr<Object>()->SetName("Ground");
        groundObj->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage00.mv1");
        groundObj->SetScaleAxisXYZ({0.5f, 0.1f, 0.5f});
        groundObj->AddComponent<ComponentCollisionModel>()->AttachToModel(true);

        auto skyboxObj = Scene::CreateObjectPtr<Object>()->SetName("Skybox");
        skyboxObj->SetTranslate({0, 0, 0});
        skyboxObj->SetRotationAxisXYZ({0, 180, 0});
        skyboxObj->AddComponent<ComponentModel>("data/LittleQuest/Model/Skybox.mv1")->SetScaleAxisXYZ(100.0f);

        {
            auto broken = BrokenHouse::Create("BrokenHouse", {205, 7, -179});
            broken->SetScaleAxisXYZ(2);
            broken->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto broken = BrokenHouse::Create("BrokenHouse", {124, 6.5, -179});
            broken->SetScaleAxisXYZ(2);
            broken->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto broken = BrokenHouse::Create("BrokenHouse", {51, 5.5f, -179});
            broken->SetScaleAxisXYZ(2);
            broken->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto broken = BrokenHouse::Create("BrokenHouse", {-23, 2.5f, -179});
            broken->SetScaleAxisXYZ(2);
            broken->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto broken = BrokenHouse::Create("BrokenHouse", {205, 8, -259});
            broken->SetScaleAxisXYZ(2);
            broken->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto broken = BrokenHouse::Create("BrokenHouse", {124, 7, -259});
            broken->SetScaleAxisXYZ(2);
            broken->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto broken = BrokenHouse::Create("BrokenHouse", {51, 4.5f, -259});
            broken->SetScaleAxisXYZ(2);
            broken->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto broken = BrokenHouse::Create("BrokenHouse", {-23, 2.5f, -259});
            broken->SetScaleAxisXYZ(2);
            broken->SetRotationAxisXYZ({0, 90, 0});
        }

        //205 8 -259
        //124 7 -259
        //4.5
        //2.5
        {
            auto Destroyed = DestroyedHouse::Create("DestroyedHouse", {205, 0, 90});
            Destroyed->SetScaleAxisXYZ(2);
            Destroyed->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto Destroyed = DestroyedHouse::Create("DestroyedHouse", {124, 0.5f, 90});
            Destroyed->SetScaleAxisXYZ(2);
            Destroyed->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto Destroyed = DestroyedHouse::Create("DestroyedHouse", {51, 0.6f, 90});
            Destroyed->SetScaleAxisXYZ(2);
            Destroyed->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto Destroyed = DestroyedHouse::Create("DestroyedHouse", {-23, 2.5f, 90});
            Destroyed->SetScaleAxisXYZ(2);
            Destroyed->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto Destroyed = DestroyedHouse::Create("DestroyedHouse", {205, 0, 175});
            Destroyed->SetScaleAxisXYZ(2);
            Destroyed->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto Destroyed = DestroyedHouse::Create("DestroyedHouse", {124, 0.5f, 175});
            Destroyed->SetScaleAxisXYZ(2);
            Destroyed->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto Destroyed = DestroyedHouse::Create("DestroyedHouse", {51, 0.6f, 175});
            Destroyed->SetScaleAxisXYZ(2);
            Destroyed->SetRotationAxisXYZ({0, 90, 0});
        }
        {
            auto Destroyed = DestroyedHouse::Create("DestroyedHouse", {-23, 2.5f, 175});
            Destroyed->SetScaleAxisXYZ(2);
            Destroyed->SetRotationAxisXYZ({0, 90, 0});
        }
        //175
    }

    m_pPlayer = Player::Create(PLAYER_SPAWN_POS);
    m_pPlayer.lock()->SetSceneState(scene_state);

    m_pPlayerCamera = Camera::Create(m_pPlayer.lock());
    m_pPlayerCamera.lock()->SetName("PlayerCamera");

    m_pBoss = Boss::Create(BOSS_SPAWN_POS);
    m_pBoss.lock()->SetRotationAxisXYZ({0, 90, 0});
    m_pBoss.lock()->SetSceneState(scene_state);

    auto obj  = Scene::CreateObjectPtr<Object>()->SetName("CutSceneCamera");
    m_pCamera = obj->AddComponent<ComponentCamera>();
    m_pCamera.lock()->SetCurrentCamera();
    m_pCamera.lock()->SetPositionAndTarget(CUT_SCENE_POS_1, m_pBoss.lock()->GetTranslate() + float3{0, 20, 0});
    m_pCamera.lock()->SetPerspective(FOV_1);
    //85 20 -50
    // 120
    //SetUseLighting(TRUE);
    //SetGlobalAmbientLight(GetColorF(0, 0, 150, 255));
#ifndef _DEBUG
    HideMouse(true);
#endif    // !_DEBUG

    return true;
}

//---------------------------------------------------------------------------
//! 更新
//! @param  [in]    delta   経過時間
//---------------------------------------------------------------------------
void        Stage01::Update() {
#if defined _DEBUG
    /* if(IsKeyDown(KEY_INPUT_INSERT)) {
        auto enemy = Zombie::Create({10, 20, 10});
        enemies.push_back(enemy);
    }

    if(IsKeyDown(KEY_INPUT_HOME)) {
        auto enemy = Mutant::Create({15, 20, 15});
        enemies.push_back(enemy);
    }*/
#endif
    float  t;
    float3 newCamPos;
    float3 newCamTarget;
    float  newFOV;

    switch(scene_state) {
    case Scene::SceneState::TRANS_IN:
        if(FadeIn()) {
            m_pBoss.lock()->PlayTaunt();
        }

        if(m_pBoss.lock()->IsPlayedTaunt()) {
            m_cutSceneTimer -= GetDeltaTime60();
            m_cutSceneTimer = std::max(0.0f, m_cutSceneTimer);
            t               = abs(1 - (m_cutSceneTimer / START_CUT_SCENE_TIME));
            newCamPos       = lerp(CUT_SCENE_POS_1, m_pPlayerCamera.lock()->GetTranslate(), t);
            newCamTarget =
                lerp(m_pBoss.lock()->GetTranslate() + float3{0, 20, 0}, m_pPlayer.lock()->GetTranslate() + float3{0, 5, 0}, t);
            newFOV = lerp(float1(FOV_1), ORG_FOV, t);
            m_pCamera.lock()->SetPositionAndTarget(newCamPos, newCamTarget);
            m_pCamera.lock()->SetPerspective(newFOV);
        }

        if(m_cutSceneTimer <= 0) {
            m_pPlayerCamera.lock()->SetCurrentCamera();
            scene_state = Scene::SceneState::GAME;
            m_pPlayer.lock()->SetSceneState(scene_state);
            m_pBoss.lock()->SetSceneState(scene_state);
        }

        if(IsKeyDown(KEY_INPUT_P)) {
            //m_pPlayerCamera.lock()->SetCurrentCamera();
            //scene_state = Scene::SceneState::GAME;
        }
        break;
    case Scene::SceneState::GAME:
        if((m_pBoss.lock()->IsDead() || m_pPlayer.lock()->IsDead()) && FadeOut()) {
            scene_state = Scene::SceneState::TRANS_OUT;
            m_pPlayer.lock()->SetSceneState(scene_state);
            m_pBoss.lock()->SetSceneState(scene_state);
            m_pPlayer.lock()->SetTranslate(PLAYER_SPAWN_POS);
            m_pBoss.lock()->SetTranslate(BOSS_SPAWN_POS);
        }
        break;
    case Scene::SceneState::TRANS_OUT:
        m_pBoss.lock()->PlayDead();
        m_pPlayer.lock()->PlayDead();

        m_pCamera.lock()->SetCurrentCamera();
        if(m_pBoss.lock()->IsDead()) {
            m_pCamera.lock()->SetPositionAndTarget(BOSS_DEATH_CAM, m_pBoss.lock()->GetTranslate() + float3{0, 15, 0});
            m_showImage = m_clearImage;
        } else {
            m_pCamera.lock()->SetPositionAndTarget(PLAYER_DEATH_CAM, m_pPlayer.lock()->GetTranslate() + float3{0, 10, 0});
            m_showImage = m_failImage;
        }
        break;
    }

    //if(M_IsCutScene) {
    //    m_pPlayer.lock()->SetStatus(Object::StatusBit::NoDraw, M_IsCutScene);
    //}

    // if (IsKeyRepeat(KEY_INPUT_SPACE)) {
    //     HideMouse(false);
    //     if(IsKeyRelease(KEY_INPUT_SPACE)) {
    //         HideMouse(true);
    //     }
    // }

    //for(int i = 0; i < enemies.size(); i++) {
    //    if(enemies[i]->getDestroyTimer() <= 0) {
    //        Scene::ReleaseObject(enemies[i]);
    //        enemies.erase(enemies.begin() + i);
    //    }
    //}

    //if(GetMainWindowHandle() == GetForegroundWindow()) {
    //    HideMouse(true);
    //} else {
    //    HideMouse(false);
    //}

    //if(enemies.size() == 0) {
    //    Scene::Change(Scene::GetScene<GameWinScene>());
    //}
}

//---------------------------------------------------------------------------
//! 描画
//---------------------------------------------------------------------------
void Stage01::Draw() {}
void Stage01::LateDraw() {
    if(Scene::IsEdit()) {
        //printfDx("\nplayed taunt: %i", m_pBoss.lock()->IsPlayedTaunt());
    }
    int screen_width, screen_height;
    GetScreenState(&screen_width, &screen_height, NULL);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)m_alpha);
    DrawBox(0, 0, screen_width, screen_height, 0u, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NULL);

    switch(scene_state) {
    case Scene::SceneState::TRANS_IN:
        break;
    case Scene::SceneState::GAME:
        break;
    case Scene::SceneState::TRANS_OUT:
        if(FadeIn()) {
            if(ShowMessage()) {
                DrawStringToHandle((int)((screen_width * 0.5f) - (m_stringWidth * 0.5f)), (int)(screen_height * 0.8),
                                   "Press any key back to Title", 0xffee42, m_fontHandle, 0xffaf3f);
                if(IsKeyDown(KEY_INPUT_RETURN) || IsMouseDown(MOUSE_INPUT_1)) {
                    Scene::Change(Scene::GetScene<GameTitleScene>());
                }
            } else {
                if(IsKeyDown(KEY_INPUT_RETURN) || IsMouseDown(MOUSE_INPUT_1)) {
                    m_shrinkTimer = SHRINK_TIME;
                }
            }

            DrawExtendGraph((int)(screen_width * 0.2f), (int)(screen_height * (0.0f + (0.4f * (m_shrinkTimer / SHRINK_TIME)))),
                            (int)(screen_width * 0.8f), (int)(screen_height * (1.0f - (0.4f * (m_shrinkTimer / SHRINK_TIME)))),
                            m_showImage, TRUE);
        }
        break;
    }
}

//---------------------------------------------------------------------------
//! 終了
//---------------------------------------------------------------------------
void Stage01::Exit() {}

//---------------------------------------------------------------------------
//! GUI表示
//---------------------------------------------------------------------------
void Stage01::GUI() {}

bool Stage01::FadeIn() {
    if(m_fadeTimer > 0) {
        m_fadeTimer -= GetDeltaTime60();
        m_alpha = abs(m_fadeTimer / FADE_TIME) * 255;
    }

    return m_fadeTimer <= 0;
}

bool Stage01::FadeOut() {
    if(m_fadeTimer < FADE_TIME) {
        m_fadeTimer += GetDeltaTime60();
        m_alpha = abs(m_fadeTimer / FADE_TIME) * 255;
    }

    return m_fadeTimer >= FADE_TIME;
}

bool Stage01::ShowMessage() {
    if(m_shrinkTimer < SHRINK_TIME) {
        m_shrinkTimer += GetDeltaTime60();
    }
    return m_shrinkTimer >= SHRINK_TIME;
}
}    // namespace LittleQuest

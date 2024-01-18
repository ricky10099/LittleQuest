//---------------------------------------------------------------------------
//! @file   Stage01.cpp
//! @brief  Stage01シーン
//---------------------------------------------------------------------------
#include "Stage01.h"

#include <LittleQuest/Objects/Camera.h>
#include <LittleQuest/Objects/AbandonHouse.h>
#include <LittleQuest/Objects/BrokenHouse.h>
#include <LittleQuest/Objects/DestroyedHouse.h>
#include <LittleQuest/Objects/Rock1.h>
#include <LittleQuest/Objects/Fence.h>
#include <LittleQuest/Scenes/GameOverScene.h>
#include <LittleQuest/Scenes/GameWinScene.h>
#include <LittleQuest/Scenes/GameTitleScene.h>
#include <LittleQuest/Components/ComponentTexture.h>

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

extern int bgm_volume;

namespace LittleQuest {

BP_CLASS_IMPL(Stage01, u8"LittleQuest/Stage01")

//===========================================================================
//  Stage01シーン
//===========================================================================

//---------------------------------------------------------------------------
//! 初期化
//---------------------------------------------------------------------------
bool Stage01::Init() {
    m_fontHandle = CreateFontToHandle("M PLUS Code Latin", 55, 4, DX_FONTTYPE_ANTIALIASING_EDGE, DX_CHARSET_UTF8, 3);
    GetDrawStringSizeToHandle(&m_stringWidth, &m_stringHeight, NULL, "Press any key back to Title", -1, m_fontHandle);
    m_timerFontHandle = CreateFontToHandle("M PLUS Code Latin", 80, 4, DX_FONTTYPE_ANTIALIASING_EDGE, DX_CHARSET_UTF8, 1);
    GetDrawStringSizeToHandle(&m_timerWidth, &m_timerHeight, NULL, "88:88.888", -1, m_timerFontHandle);
    m_clearImage = LoadGraph("data/LittleQuest/Image/clear.png");
    m_failImage  = LoadGraph("data/LittleQuest/Image/failure.png");
    {
        auto clearObj = Scene::CreateObjectPtr<Object>()->SetName("ClearTexture");
        m_pClearImage = clearObj->AddComponent<ComponentTexture2D>("data/LittleQuest/Image/Clear.png");
    }
    {
        auto failObj = Scene::CreateObjectPtr<Object>()->SetName("FailTexture");
        m_pFailImage = failObj->AddComponent<ComponentTexture2D>("data/LittleQuest/Image/Failure.png");
    }
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
        {
            auto AbandonHouse = AbandonHouse::Create("AbandonHouse", {-205, 2.4, 136});
            AbandonHouse->SetRotationAxisXYZ({0, 90, 0});
            AbandonHouse->SetScaleAxisXYZ(2.5f);
        }
        {
            auto AbandonHouse = AbandonHouse::Create("AbandonHouse", {-205, -0.15, 26});
            AbandonHouse->SetRotationAxisXYZ({0, 90, 0});
            AbandonHouse->SetScaleAxisXYZ(2.5f);
        }
        {
            auto AbandonHouse = AbandonHouse::Create("AbandonHouse", {-205, -0.5, -103});
            AbandonHouse->SetRotationAxisXYZ({0, 90, 0});
            AbandonHouse->SetScaleAxisXYZ(2.5f);
        }
        {
            auto AbandonHouse = AbandonHouse::Create("AbandonHouse", {-205, 0.165, -197});
            AbandonHouse->SetRotationAxisXYZ({0, 90, 0});
            AbandonHouse->SetScaleAxisXYZ(2.5f);
        }
        {
            auto AbandonHouse = AbandonHouse::Create("AbandonHouse", {-110, 1.5, 136});
            AbandonHouse->SetRotationAxisXYZ({0, 90, 0});
            AbandonHouse->SetScaleAxisXYZ(2.5f);
        }
        {
            auto AbandonHouse = AbandonHouse::Create("AbandonHouse", {-110, -0.2, 26});
            AbandonHouse->SetRotationAxisXYZ({0, 90, 0});
            AbandonHouse->SetScaleAxisXYZ(2.5f);
        }
        {
            auto AbandonHouse = AbandonHouse::Create("AbandonHouse", {-110, -0.5, -103});
            AbandonHouse->SetRotationAxisXYZ({0, 90, 0});
            AbandonHouse->SetScaleAxisXYZ(2.5f);
        }
        {
            auto AbandonHouse = AbandonHouse::Create("AbandonHouse", {-110, 3.5, -197});
            AbandonHouse->SetRotationAxisXYZ({0, 90, 0});
            AbandonHouse->SetScaleAxisXYZ(2.5f);
        }
        {
            auto Rock = Rock1::Create({330, -13, -62});
            Rock->SetScaleAxisXYZ({1.0f, 1.0f, 3.0f});
        }
        {
            auto Rock = Rock1::Create({9, -4, 260});
            Rock->SetRotationAxisXYZ({0, 90, 0});
            Rock->SetScaleAxisXYZ({1.0f, 1.0f, 3.0f});
        }
        {
            auto Rock = Rock1::Create({20, 5, -364});
            Rock->SetRotationAxisXYZ({0, 90, 0});
            Rock->SetScaleAxisXYZ({1.0f, 1.0f, 3.0f});
        }

        int interval = 50;
        for(int i = -315; i < 210; i += interval) {
            auto Fence = Fence::Create({-255, 6, i});
            Fence->SetRotationAxisXYZ({0, 90, 0});
        }
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
    m_pCamera.lock()->SetPerspective(FOV_INTRO);

    m_introBGM  = LoadSoundMem("data/LittleQuest/Audio/BGM/IntroBGM_long.mp3");
    m_BGM       = LoadSoundMem("data/LittleQuest/Audio/BGM/Thunder_of_God.mp3");
    m_loseAudio = LoadSoundMem("data/LittleQuest/Audio/BGM/Lose.mp3");

    Scene::SetSceneBGMList({
        {m_introBGM, DX_PLAYTYPE_BACK},
        {     m_BGM, DX_PLAYTYPE_LOOP}
    });
    Scene::QueueScene(Scene::GetScene<GameTitleScene>());
    Scene::SetCanPause(true);
    return true;
}

//---------------------------------------------------------------------------
//! 更新
//! @param  [in]    delta   経過時間
//---------------------------------------------------------------------------
void Stage01::Update() {
    float  t;
    float3 newCamPos;
    float3 newCamTarget;
    float  newFOV;

#ifndef _DEBUG
    HideMouse(true);
#endif    // !_DEBUG

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
            newFOV = lerp(float1(FOV_INTRO), FOV_ORG, t);
            m_pCamera.lock()->SetPositionAndTarget(newCamPos, newCamTarget);
            m_pCamera.lock()->SetPerspective(newFOV);
        }

        if(m_cutSceneTimer <= 0) {
            m_pPlayerCamera.lock()->SetCurrentCamera();
            scene_state = Scene::SceneState::GAME;
            m_pPlayer.lock()->SetSceneState(scene_state);
            m_pBoss.lock()->SetSceneState(scene_state);
        }

        if(IsKeyDown(KEY_INPUT_RETURN) || IsMouseDown(MOUSE_INPUT_1) || IsKeyDown(KEY_INPUT_SPACE) ||
           /*IsPadDown(PAD_ID::PAD_10, DX_PADTYPE_DUAL_SENSE) || */ IsPadDown(PAD_ID::PAD_3, DX_PADTYPE_DUAL_SENSE)) {
            m_fadeTimer     = 0;
            m_alpha         = 0;
            m_cutSceneTimer = 0;
            m_pCamera.lock()->SetPerspective(FOV_ORG);
        }
        break;
    case Scene::SceneState::GAME:
        m_second -= GetDeltaTime();
        if(m_second <= 0) {
            if(m_minute <= 0) {
                m_isLose = true;
                m_second = 0.0f;
            } else {
                m_second = 59.99f;
            }
            m_minute--;
            m_minute = std::max(0, m_minute);
        }
        if((m_pBoss.lock()->IsDead() || m_pPlayer.lock()->IsDead() || m_isLose) && FadeOut()) {
            scene_state = Scene::SceneState::TRANS_OUT;
            m_pPlayer.lock()->SetSceneState(scene_state);
            m_pBoss.lock()->SetSceneState(scene_state);
            m_pPlayer.lock()->SetTranslate(PLAYER_SPAWN_POS);
            m_pBoss.lock()->SetTranslate(BOSS_SPAWN_POS);
            m_pBoss.lock()->SetRotationAxisXYZ({0, 90, 0});
        }
        break;
    case Scene::SceneState::TRANS_OUT:
        Scene::SetCanPause(false);
        m_endingTimer -= GetDeltaTime60();

        StopSoundMem(m_BGM);
        StopSoundMem(m_introBGM);
        m_pBoss.lock()->PlayDead();
        m_pPlayer.lock()->PlayDead();

        m_pCamera.lock()->SetCurrentCamera();
        if(m_pBoss.lock()->IsDead()) {
            m_pCamera.lock()->SetPositionAndTarget(BOSS_DEATH_CAM, m_pBoss.lock()->GetTranslate() + float3{0, 15, 0});
            m_showImage  = m_clearImage;
            m_pShowImage = m_pClearImage.lock();
        } else {
            m_pCamera.lock()->SetPositionAndTarget(PLAYER_DEATH_CAM, m_pPlayer.lock()->GetTranslate() + float3{0, 10, 0});
            m_showImage  = m_failImage;
            m_pShowImage = m_pFailImage.lock();
            PlaySoundMem(m_loseAudio, DX_PLAYTYPE_BACK);
        }
        break;
    }
}

//---------------------------------------------------------------------------
//! 描画
//---------------------------------------------------------------------------
void Stage01::Draw() {}
void Stage01::LateDraw() {
    int screen_width, screen_height;
    GetScreenState(&screen_width, &screen_height, NULL);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)m_alpha);
    DrawBox(0, 0, screen_width, screen_height, 0u, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NULL);
    int timerColor = 0xffffff;
    if(m_minute <= 0) {
        timerColor = 0xff0000;
    }

    switch(scene_state) {
    case Scene::SceneState::TRANS_IN:
        break;
    case Scene::SceneState::GAME:
        DrawFormatStringToHandle((int)((screen_width * 0.9f) - (m_stringWidth * 0.5f)), (int)(screen_height * 0.1), timerColor,
                                 m_timerFontHandle, "%02i:%06.3f", m_minute, m_second);
        break;
    case Scene::SceneState::TRANS_OUT:
        if(FadeIn()) {
            if(ShowMessage()) {
                DrawStringToHandle((int)((screen_width * 0.5f) - (m_stringWidth * 0.5f)), (int)(screen_height * 0.8),
                                   "Press any key back to Title", 0xffee42, m_fontHandle, 0xffaf3f);
                if(IsKeyDown(KEY_INPUT_RETURN) || IsMouseDown(MOUSE_INPUT_1) || IsKeyDown(KEY_INPUT_SPACE) ||
                   IsKeyDown(KEY_INPUT_ESCAPE) || IsPadDown(PAD_ID::PAD_10, DX_PADTYPE_DUAL_SENSE) ||
                   IsPadDown(PAD_ID::PAD_3, DX_PADTYPE_DUAL_SENSE) || m_endingTimer <= 0) {
                    Scene::Change(Scene::GetScene<GameTitleScene>());
                }
            } else {
                if(IsKeyDown(KEY_INPUT_RETURN) || IsMouseDown(MOUSE_INPUT_1) || IsKeyDown(KEY_INPUT_SPACE) ||
                   IsKeyDown(KEY_INPUT_ESCAPE) || IsPadDown(PAD_ID::PAD_10, DX_PADTYPE_DUAL_SENSE) ||
                   IsPadDown(PAD_ID::PAD_3, DX_PADTYPE_DUAL_SENSE)) {
                    m_shrinkTimer = SHRINK_TIME;
                }
            }
            m_pShowImage.lock()->SetPosition(
                (screen_width * 0.2f), (screen_height * (0.0f + (0.4f * (m_shrinkTimer / SHRINK_TIME)))), (screen_width * 0.8f),
                (screen_height * (1.0f - (0.4f * (m_shrinkTimer / SHRINK_TIME)))));
            m_pShowImage.lock()->DrawTexture();
        }
        break;
    }
}

//---------------------------------------------------------------------------
//! 終了
//---------------------------------------------------------------------------
void Stage01::Exit() {
    if(CheckSoundMem(m_introBGM)) {
        StopSoundMem(m_introBGM);
    }
    if(CheckSoundMem(m_BGM)) {
        StopSoundMem(m_BGM);
    }

    DeleteSoundMem(m_introBGM);
    DeleteSoundMem(m_BGM);
}

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

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
#include <LittleQuest/Scenes/GameTitleScene.h>
#include <LittleQuest/Components/ComponentTexture.h>

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>

extern int bg_volume;

namespace LittleQuest {
//===========================================================================
//  Stage01シーン
//===========================================================================

//---------------------------------------------------------------------------
//! 初期化
//---------------------------------------------------------------------------
bool Stage01::Init() {
    _fontHandle = CreateFontToHandle("M PLUS Code Latin", 55, 4, DX_FONTTYPE_ANTIALIASING_EDGE, DX_CHARSET_UTF8, 3);
    GetDrawStringSizeToHandle(&_stringWidth, &_stringHeight, NULL, "Press any key back to Title", -1, _fontHandle);
    _timerFontHandle = CreateFontToHandle("M PLUS Code Latin", 80, 4, DX_FONTTYPE_ANTIALIASING_EDGE, DX_CHARSET_UTF8, 1);
    GetDrawStringSizeToHandle(&_timerWidth, &_timerHeight, NULL, "88:88.888", -1, _timerFontHandle);

    {
        auto clearObj = Scene::CreateObjectPtr<Object>()->SetName("ClearTexture");
        _pClearImage  = clearObj->AddComponent<ComponentTexture2D>("data/LittleQuest/Image/Clear.png");
    }
    {
        auto failObj = Scene::CreateObjectPtr<Object>()->SetName("FailTexture");
        _pFailImage  = failObj->AddComponent<ComponentTexture2D>("data/LittleQuest/Image/Failure.png");
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

        for(int i = 0; i < sizeof(ABANDONHOUSE_POS) / sizeof(*ABANDONHOUSE_POS); ++i) {
            auto broken = BrokenHouse::Create("BrokenHouse", BROKENHOUSE_POS[i]);
            broken->SetScaleAxisXYZ(2);
            broken->SetRotationAxisXYZ({0, 90, 0});

            auto Destroyed = DestroyedHouse::Create("DestroyedHouse", DESTROYEDHOUSE_POS[i]);
            Destroyed->SetScaleAxisXYZ(2);
            Destroyed->SetRotationAxisXYZ({0, 90, 0});

            auto AbandonHouse = AbandonHouse::Create("AbandonHouse", ABANDONHOUSE_POS[i]);
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

    _pPlayer = Player::Create(PLAYER_SPAWN_POS);
    _pPlayer.lock()->SetSceneState(scene_state);

    _pPlayerCamera = Camera::Create(_pPlayer.lock());
    _pPlayerCamera.lock()->SetName("PlayerCamera");
    _pPlayerCamera.lock()->SetTranslate({-97, 17, -50});

    _pBoss = MawJLaygo::Create(BOSS_SPAWN_POS);
    _pBoss.lock()->SetRotationAxisXYZ({0, 90, 0});
    _pBoss.lock()->SetSceneState(scene_state);

    //for(auto& mob: _pMob) {
    //    mob = Mutant::Create(MOB_SPAWN_POS, false);
    //    mob.lock()->SetSpawnPoint(MOB_SPAWN_POS);
    //    mob.lock()->SetRotationAxisXYZ({0, 90, 0});
    //    mob.lock()->SetSceneState(scene_state);
    //}
    for(int i = 0; i < 4; ++i) {
        _pMob[i] = Mutant::Create(MOB_POS[i], false);
        _pMob[i].lock()->SetSpawnPoint(MOB_POS[i]);
        _pMob[i].lock()->SetRotationAxisXYZ({0, 90, 0});
        _pMob[i].lock()->SetSceneState(scene_state);
        _pMob[i].lock()->SetDetectDistance(200);
    }

    auto obj = Scene::CreateObjectPtr<Object>()->SetName("CutSceneCamera");
    _pCamera = obj->AddComponent<ComponentCamera>();
    _pCamera.lock()->SetCurrentCamera();
    _pCamera.lock()->SetPositionAndTarget(CUT_SCENE_POS_START, _pBoss.lock()->GetTranslate() + float3{0, 20, 0});
    _pCamera.lock()->SetPerspective(FOV_INTRO);

    _introBGM = LoadSoundMem("data/LittleQuest/Audio/BGM/IntroBG_long.mp3");
    _BGM      = LoadSoundMem("data/LittleQuest/Audio/BGM/Thunder_of_God.mp3");

    Scene::SetSceneBGMList({
        {_introBGM, DX_PLAYTYPE_BACK},
        {     _BGM, DX_PLAYTYPE_LOOP}
    });
    Scene::QueueScene(Scene::GetScene<GameTitleScene>());
    Scene::SetCanPause(true);
    return true;
}

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
            _pBoss.lock()->PlayTaunt();
            _pPlayerCamera.lock()->SetTranslate({-97, 17, -50});
        }

        ShowBlackBar();

        if(_pBoss.lock()->IsPlayedTaunt()) {
            for(auto& mob: _pMob) {
                if(!mob.lock()->GetIsReady()) {
                    mob.lock()->SetToSpawnState();
                }
            }
            _mobSpawnTimer -= GetDeltaTime60();
            if(_mobSpawnTimer <= 0) {
                _isMobSpawned = true;
            }
            _pCamera.lock()->SetPositionAndTarget(CUT_SCENE_POS_MOB, CUT_SCENE_TARGET_MOB);
        }

        if(_isMobSpawned) {
            _slideBlackBar = true;
            _pPlayerCamera.lock()->SetTranslate({-97, 17, -50});
            _cutSceneTimer -= GetDeltaTime60();
            _cutSceneTimer = std::max(0.0f, _cutSceneTimer);
            t              = abs(1 - (_cutSceneTimer / START_CUT_SCENE_TIME));
            newCamPos      = lerp(CUT_SCENE_POS_MOB, _pPlayerCamera.lock()->GetTranslate(), t);
            newCamTarget   = lerp(CUT_SCENE_TARGET_MOB, _pPlayer.lock()->GetTranslate(), t);
            newFOV         = lerp(float1(FOV_INTRO), FOV_ORG, t);
            _pCamera.lock()->SetPositionAndTarget(newCamPos, newCamTarget);

            _pCamera.lock()->SetPerspective(newFOV);
        }

        if(_cutSceneTimer <= 0) {
            _pPlayerCamera.lock()->SetCurrentCamera();
            scene_state = Scene::SceneState::GAME;
            _pPlayer.lock()->SetSceneState(scene_state);
            _pBoss.lock()->SetSceneState(scene_state);
            for(auto& mob: _pMob) {
                mob.lock()->SetSceneState(scene_state);
            }
        }

        if(IsKeyDown(KEY_INPUT_RETURN) /*|| IsMouseDown(MOUSE_INPUT_1)*/ || IsKeyDown(KEY_INPUT_SPACE) ||
           IsPadOn(PAD_ID::PAD_L_PUSH) || IsPadOn(PAD_ID::PAD_B)) {
            _fadeTimer     = 0;
            _alpha         = 0;
            _cutSceneTimer = 0;
            _pCamera.lock()->SetPerspective(FOV_ORG);
        }
        break;
    case Scene::SceneState::GAME:

        //#ifndef _DEBUG
        _second -= GetDeltaTime();
        //#endif    // !_DEBUG
        _showBlackBar = false;
        if(_second <= 0) {
            if(_minute <= 0) {
                _isLose = true;
                _second = 0.0f;
            } else {
                _second = 59.99f;
            }
            _minute--;
            _minute = std::max(0, _minute);
        }
        if(_pBoss.lock()->IsDead() || _pPlayer.lock()->IsDead() || _isLose) {
            _showBlackBar  = true;
            _slideBlackBar = false;
            if(_cutSceneTimer == START_CUT_SCENE_TIME) {
                _pPlayer.lock()->SlowMotion();
                _pBoss.lock()->SlowMotion();
            }
            _cutSceneTimer -= GetDeltaTime60();
            _cutSceneTimer = std::max(0.0f, _cutSceneTimer);
            _pCamera.lock()->SetCurrentCamera();
            if(_cutSceneTimer > 120.0f) {
                _pCamera.lock()->SetPositionAndTarget(_pPlayer.lock()->GetTranslate() + float3{20, 15, 20},
                                                      _pPlayer.lock()->GetTranslate() + float3{0, 5, 0});
            } else {
                _pCamera.lock()->SetPositionAndTarget(_pBoss.lock()->GetTranslate() + float3{-40, 15, -40},
                                                      _pBoss.lock()->GetTranslate() + float3{0, 5, 0});
            }

            _pPlayer.lock()->SetHideUI(true);
            _pBoss.lock()->SetHideUI(true);

        } else {
            _cutSceneTimer = START_CUT_SCENE_TIME;
        }

        if(_cutSceneTimer <= 0 && FadeOut()) {
            scene_state = Scene::SceneState::TRANS_OUT;
            _pPlayer.lock()->SetSceneState(scene_state);
            _pBoss.lock()->SetSceneState(scene_state);
            //_pMob.lock()->SetSceneState(scene_state);
            _pPlayer.lock()->SetTranslate(PLAYER_SPAWN_POS);
            _pBoss.lock()->SetTranslate(BOSS_SPAWN_POS);
            _pBoss.lock()->SetRotationAxisXYZ({0, 90, 0});
            _pPlayer.lock()->EndSlowMotion();
            _pBoss.lock()->EndSlowMotion();
        }
        break;
    case Scene::SceneState::TRANS_OUT:
        _showBlackBar = false;
        Scene::SetCanPause(false);
        _endingTimer -= GetDeltaTime60();

        StopSoundMem(_BGM);
        StopSoundMem(_introBGM);
        _pBoss.lock()->PlayDead();
        _pPlayer.lock()->PlayDead();

        _pCamera.lock()->SetCurrentCamera();
        if(_pBoss.lock()->IsDead()) {
            _pCamera.lock()->SetPositionAndTarget(BOSS_DEATH_CAM, _pBoss.lock()->GetTranslate() + float3{0, 15, 0});
            _pShowImage = _pClearImage.lock();
        } else {
            _pCamera.lock()->SetPositionAndTarget(PLAYER_DEATH_CAM, _pPlayer.lock()->GetTranslate() + float3{0, 10, 0});
            _pShowImage = _pFailImage.lock();
        }
        break;
    }
}

void Stage01::LateDraw() {
    int screen_width, screen_height;
    GetScreenState(&screen_width, &screen_height, NULL);

    float blackbarY = 1.0f;
    if(_slideBlackBar) {
        blackbarY = (_cutSceneTimer / START_CUT_SCENE_TIME);
    }

    if(_showBlackBar) {
        DrawBoxAA(0, 0, screen_width, screen_height * 0.15f * blackbarY, 0u, TRUE);
        DrawBoxAA(0, screen_height - screen_height * 0.15f * blackbarY, screen_width, screen_height, 0u, TRUE);
    }

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)_alpha);
    DrawBox(0, 0, screen_width, screen_height, 0u, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NULL);
    int timerColor = 0xffffff;
    if(_minute <= 0) {
        timerColor = 0xff0000;
    }

    switch(scene_state) {
    case Scene::SceneState::TRANS_IN:
        break;
    case Scene::SceneState::GAME:
        if(!(_pBoss.lock()->IsDead() || _pPlayer.lock()->IsDead())) {
            DrawFormatStringToHandle((int)((screen_width * 0.9f) - (_stringWidth * 0.5f)), (int)(screen_height * 0.1),
                                     timerColor, _timerFontHandle, "%02i:%06.3f", _minute, _second);
        }
        break;
    case Scene::SceneState::TRANS_OUT:
        if(FadeIn()) {
            if(ShowMessage()) {
                DrawStringToHandle((int)((screen_width * 0.5f) - (_stringWidth * 0.5f)), (int)(screen_height * 0.8),
                                   "Press any key back to Title", 0xffee42, _fontHandle, 0xffaf3f);
                if(IsKeyDown(KEY_INPUT_RETURN) || IsMouseDown(MOUSE_INPUT_1) || IsKeyDown(KEY_INPUT_SPACE) ||
                   IsKeyDown(KEY_INPUT_ESCAPE) || IsPadOn(PAD_ID::PAD_R_PUSH) || IsPadOn(PAD_ID::PAD_B) || _endingTimer <= 0) {
                    Scene::Change(Scene::GetScene<GameTitleScene>());
                }
            } else {
                if(IsKeyDown(KEY_INPUT_RETURN) || IsMouseDown(MOUSE_INPUT_1) || IsKeyDown(KEY_INPUT_SPACE) ||
                   IsKeyDown(KEY_INPUT_ESCAPE) || IsPadOn(PAD_ID::PAD_R_PUSH) || IsPadOn(PAD_ID::PAD_B)) {
                    _shrinkTimer = SHRINK_TIME;
                }
            }
            _pShowImage.lock()->SetPosition(
                (screen_width * 0.2f), (screen_height * (0.0f + (0.4f * (_shrinkTimer / SHRINK_TIME)))), (screen_width * 0.8f),
                (screen_height * (1.0f - (0.4f * (_shrinkTimer / SHRINK_TIME)))));
            _pShowImage.lock()->DrawTexture();
        }
        break;
    }
}

//---------------------------------------------------------------------------
//! 終了
//---------------------------------------------------------------------------
void Stage01::Exit() {
    if(CheckSoundMem(_introBGM)) {
        StopSoundMem(_introBGM);
    }
    if(CheckSoundMem(_BGM)) {
        StopSoundMem(_BGM);
    }

    DeleteSoundMem(_introBGM);
    DeleteSoundMem(_BGM);
}

bool Stage01::FadeIn() {
    if(_fadeTimer > 0) {
        _fadeTimer -= GetDeltaTime60();
        _alpha = abs(_fadeTimer / FADE_TIME) * 255;
    }

    return _fadeTimer <= 0;
}

bool Stage01::FadeOut() {
    if(_fadeTimer < FADE_TIME) {
        _fadeTimer += GetDeltaTime60();
        _alpha = abs(_fadeTimer / FADE_TIME) * 255;
    }

    return _fadeTimer >= FADE_TIME;
}

void Stage01::ShowBlackBar() {
    _showBlackBar = true;
}

bool Stage01::ShowMessage() {
    if(_shrinkTimer < SHRINK_TIME) {
        _shrinkTimer += GetDeltaTime60();
    }
    return _shrinkTimer >= SHRINK_TIME;
}
}    // namespace LittleQuest

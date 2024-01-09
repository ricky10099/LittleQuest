#include "GameTitleScene.h"
#include "Stage01.h"

#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {
BP_CLASS_IMPL(GameTitleScene, u8"LittleQuest/GameTitleScene");

bool GameTitleScene::Init() {
    if(AddFontResourceEx("data/LittleQuest/Fonts/MPLUSCodeLatin-Regular.ttf", FR_PRIVATE, NULL) > 0) {
    } else {
        MessageBox(NULL, "フォント読込失敗", "", MB_OK);
    }
    m_image      = LoadGraph("data/LittleQuest/Image/TitleName.png");
    m_fontHandle = CreateFontToHandle("M PLUS Code Latin", 30, 4, DX_FONTTYPE_ANTIALIASING_EDGE, DX_CHARSET_UTF8, 1);
    GetDrawStringSizeToHandle(&m_stringWidth, &m_stringHeight, NULL, "Press any key to start", -1, m_fontHandle);

    {
        auto groundObj = Scene::CreateObjectPtr<Object>()->SetName("Ground");
        groundObj->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage00.mv1");
        groundObj->SetScaleAxisXYZ({0.5f, 0.5f, 0.5f});

        auto treeObj = Scene::CreateObjectPtr<Object>()->SetName("Tree");
        treeObj->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage_Obj001.mv1")
            ->SetScaleAxisXYZ({0.05f, 0.03f, 0.05f})
            ->SetTranslate({0, 2.3f, 2.2f})
            ->SetRotationAxisXYZ({0, -75, 0});

        auto skyboxObj = Scene::CreateObjectPtr<Object>()->SetName("Skybox");
        skyboxObj->SetTranslate({0, 0, 0});
        skyboxObj->SetRotationAxisXYZ({0, 180, 0});
        skyboxObj->AddComponent<ComponentModel>("data/LittleQuest/Model/Skybox.mv1")->SetScaleAxisXYZ(100.0f);
    }

    {
        auto playerObj = Scene::CreateObjectPtr<Object>()->SetName("Player");
        playerObj->SetScaleAxisXYZ({0.3f});
        playerObj->SetTranslate({0, 2.3f, 0});
        m_pModel = playerObj->AddComponent<ComponentModel>("data/LittleQuest/Model/Kachujin/Kachujin.mv1");
        m_pModel->SetAnimation({
            {  "Sit", "data/LittleQuest/Anim/KachujinSet/SittingIdle.mv1", 0, 1.0f},
            {"Stand",    "data/LittleQuest/Anim/KachujinSet/Standing.mv1", 0, 2.0f},
        });
        m_pModel->PlayAnimationNoSame("Sit", true);

        auto cameraObj = Scene::CreateObjectPtr<Object>()->SetName(u8"Camera");
        m_pCamera      = cameraObj->AddComponent<ComponentCamera>();
        m_pCamera->SetPositionAndTarget(START_CAM_POS, START_CAM_TARGET);
        auto springArm = cameraObj->AddComponent<ComponentSpringArm>();
        springArm->SetSpringArmObject(playerObj);
        springArm->SetSpringArmRotate({0, 47, 0});
        springArm->SetSpringArmOffset({0, 0, 0});
    }

    return true;
}

void GameTitleScene::Update() {
    m_elapsed60 += GetDeltaTime60();

    float  t;
    float3 newPos;
    float3 newTarget;

    switch(scene_state) {
    case Scene::SceneState::GAME:
        if(((int)m_elapsed60 % 60) == 0) {
            m_showString = !m_showString;
            m_elapsed60  = 1;
        }
        if(IsKeyOn(KEY_INPUT_RETURN)) {
            scene_state = Scene::SceneState::TRANS_OUT;
        }
        break;
    case Scene::SceneState::TRANS_OUT:
        m_startTimer -= GetDeltaTime60();
        m_startTimer = std::max(0.0f, m_startTimer);
        t            = abs(1 - (m_startTimer / START_TIME));
        newPos       = lerp(START_CAM_POS, END_CAM_POS, t);
        newTarget    = lerp(START_CAM_TARGET, END_CAM_TARGET, t);
        m_pCamera->SetPositionAndTarget(newPos, newTarget);
        m_alpha = t * 255;

        m_pModel->PlayAnimationNoSame("Stand", false, 1.0f);

        if(m_startTimer <= 0) {
            Scene::Change(Scene::GetScene<Stage01>());
        }
        break;
    }
}

void GameTitleScene::LateDraw() {
    //int percentage = (int)((m_totalLoadCount - m_loadCount) / (float)m_totalLoadCount * 100);
    int screen_width, screen_height;
    GetScreenState(&screen_width, &screen_height, NULL);

    switch(scene_state) {
    case Scene::SceneState::GAME:
        if(m_showString) {
            DrawStringToHandle((int)((screen_width * 0.5f) - (m_stringWidth * 0.5f)), (int)(screen_height * 0.8),
                               "Press any key to start", 0xffee42, m_fontHandle, 0xffaf3f);
        }

        DrawExtendGraph((int)(screen_width * 0.1f), (int)(screen_height * 0.2f), (int)(screen_width * 0.9f),
                        (int)(screen_height * 0.4f), m_image, TRUE);
        break;
    case Scene::SceneState::TRANS_OUT:
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)m_alpha);
        DrawBox(0, 0, screen_width, screen_height, 0u, TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NULL);
        DrawExtendGraph((int)(screen_width * 0.1f), (int)(screen_height * (0.2f + (0.2f * (m_alpha / 255)))),
                        (int)(screen_width * 0.9f), (int)(screen_height * (0.4f + (0.2f * (m_alpha / 255)))), m_image, TRUE);
        break;
    }
}

void GameTitleScene::Exit() {}

void GameTitleScene::GUI() {}

}    // namespace LittleQuest

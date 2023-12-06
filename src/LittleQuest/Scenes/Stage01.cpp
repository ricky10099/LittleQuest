//---------------------------------------------------------------------------
//! @file   Stage01.cpp
//! @brief  Stage01シーン
//---------------------------------------------------------------------------
#include "Stage01.h"

#include <LittleQuest/Objects/Camera.h>
#include <LittleQuest/Objects/Enemy.h>
#include <LittleQuest/Objects/Zombie.h>
#include <LittleQuest/Objects/Mutant.h>
#include <LittleQuest/Objects/Player.h>
#include <LittleQuest/Scenes/GameOverScene.h>
#include <LittleQuest/Scenes/GameWinScene.h>

#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCamera.h>
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
    // Ground
    {
        auto obj = Scene::CreateObjectPtr<Object>()->SetName("Ground");
        obj->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage00.mv1");
        if(auto cmp_mdl = obj->AddComponent<ComponentCollisionModel>())
            cmp_mdl->AttachToModel(true);
    }

    auto player = Player::Create({0, 10, 0});
    Camera::Create(player)->SetName("PlayerCamera");

    {
        auto enemy = Mutant::Create({30, 20, 15}, true);
        enemies.push_back(enemy);
    }

    HideMouse(true);

    return true;
}

//---------------------------------------------------------------------------
//! 更新
//! @param  [in]    delta   経過時間
//---------------------------------------------------------------------------
void        Stage01::Update() {
#if defined _DEBUG
    if(IsKeyDown(KEY_INPUT_INSERT)) {
        auto enemy = Zombie::Create({10, 20, 10});
        enemies.push_back(enemy);
    }

    if(IsKeyDown(KEY_INPUT_HOME)) {
        auto enemy = Mutant::Create({15, 20, 15});
        enemies.push_back(enemy);
    }
#endif

    for(int i = 0; i < enemies.size(); i++) {
        if(enemies[i]->getDestroyTimer() <= 0) {
            Scene::ReleaseObject(enemies[i]);
            enemies.erase(enemies.begin() + i);
        }
    }

    if(enemies.size() == 0) {
        Scene::Change(Scene::GetScene<GameWinScene>());
    }
}

//---------------------------------------------------------------------------
//! 描画
//---------------------------------------------------------------------------
void Stage01::Draw() {}

//---------------------------------------------------------------------------
//! 終了
//---------------------------------------------------------------------------
void Stage01::Exit() {}

//---------------------------------------------------------------------------
//! GUI表示
//---------------------------------------------------------------------------
void Stage01::GUI() {}
}    // namespace LittleQuest

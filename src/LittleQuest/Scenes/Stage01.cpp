//---------------------------------------------------------------------------
//! @file   Stage01.cpp
//! @brief  Stage01シーン
//---------------------------------------------------------------------------
#include "Stage01.h"

#include <LittleQuest/Objects/Camera.h>
#include <LittleQuest/Objects/Player.h>
#include <LittleQuest/Objects/Enemy.h>

#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentAttachModel.h>

namespace LittleQuest
{

BP_CLASS_IMPL(Stage01, u8"LittleQuest/Stage01")

//===========================================================================
//  Stage01シーン
//===========================================================================

//---------------------------------------------------------------------------
//! 初期化
//---------------------------------------------------------------------------
bool Stage01::Init()
{
    // Camera
    /*  {
              auto obj = Scene::CreateObjectPtr<Object>()->SetName(u8"Camera");
              auto cam = obj->AddComponent<ComponentCamera>();
              cam->SetPositionAndTarget({0, 20, 50}, {0, 10, 0});
          }*/

    // Ground
    {
        auto obj = Scene::CreateObjectPtr<Object>()->SetName("Ground");
        obj->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage00.mv1");
        if(auto cmp_mdl = obj->AddComponent<ComponentCollisionModel>())
            cmp_mdl->AttachToModel(true);
    }

    auto player = Player::Create({0, 10, 0});
    Camera::Create(player)->SetName("PlayerCamera");

    auto enemy = Enemy::Create({10, 20, 10});

    return true;
}

//---------------------------------------------------------------------------
//! 更新
//! @param  [in]    delta   経過時間
//---------------------------------------------------------------------------
void Stage01::Update()
{
}

//---------------------------------------------------------------------------
//! 描画
//---------------------------------------------------------------------------
void Stage01::Draw()
{
}

//---------------------------------------------------------------------------
//! 終了
//---------------------------------------------------------------------------
void Stage01::Exit()
{
}

//---------------------------------------------------------------------------
//! GUI表示
//---------------------------------------------------------------------------
void Stage01::GUI()
{
}
}   // namespace LittleQuest

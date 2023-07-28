//---------------------------------------------------------------------------
//! @file   Stage01.cpp
//! @brief  Stage01シーン
//---------------------------------------------------------------------------
#include "Stage01.h"
#include "../Objects/Camera.h"
#include <LittleQuest/Objects/Player.h>

#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentAttachModel.h>

namespace LittleQuest {

    BP_CLASS_IMPL(Stage01, u8"Stage01")

    //===========================================================================
    //  Stage01シーン
    //===========================================================================

    //---------------------------------------------------------------------------
    //! 初期化
    //---------------------------------------------------------------------------
    bool Stage01::Init() {
        // Camera
        /*  {
              auto obj = Scene::CreateObjectPtr<Object>()->SetName(u8"Camera");
              auto cam = obj->AddComponent<ComponentCamera>();
              cam->SetPositionAndTarget({0, 20, 50}, {0, 10, 0});
          }*/

        // Ground
        {
            auto obj = Scene::CreateObjectPtr<Object>()->SetName("Ground");
            obj->AddComponent<ComponentModel>(
                "data/Sample/SwordBout/Stage/Stage00.mv1");
            if (auto cmp_mdl = obj->AddComponent<ComponentCollisionModel>())
                cmp_mdl->AttachToModel(true);
        }

        auto player = Player::Create({0, 100, 0});
        Camera::Create(player)->SetName("PlayerCamera");

        auto obj = Scene::CreateObjectPtr<Object>()->SetName("Sword");
        obj->SetTranslate({0, 0, 0});
        // オブジェクトにモデルをつける
        if (auto model = obj->AddComponent<ComponentModel>()) {
            model->Load("data/LittleQuest/Model/Sword/Sword.mv1");
            // model->Load("data/Sample/FPS_Knife/Knife_low.mv1");
            model->SetRotationAxisXYZ({0, 0, 0});
            model->SetScaleAxisXYZ({0.003f, 0.003f, 0.003f});
        }

        if (auto attach = obj->AddComponent<ComponentAttachModel>()) {
            // playerの右手にアタッチする
            attach->SetAttachObject(player, "mixamorig:RightHand");
            // Knife回転量
            attach->SetAttachRotate({0, -60, 180});
            // Knifeオフセット
            attach->SetAttachOffset({13, 10, 5});
        }

        return true;
    }

    //---------------------------------------------------------------------------
    //! 更新
    //! @param  [in]    delta   経過時間
    //---------------------------------------------------------------------------
    void Stage01::Update() {}

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

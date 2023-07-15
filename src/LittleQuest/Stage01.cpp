//---------------------------------------------------------------------------
//! @file   Stage01.cpp
//! @brief  Stage01シーン
//---------------------------------------------------------------------------
#include "Stage01.h"
#include "Objects/Player.h"

#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentAttachModel.h>

BP_CLASS_IMPL(Stage01, u8"Stage01")

//===========================================================================
//  Stage01シーン
//===========================================================================

//---------------------------------------------------------------------------
//! 初期化
//---------------------------------------------------------------------------
bool Stage01::Init()
{
    // Camera
    {
        auto obj = Scene::CreateObjectPtr<Object>()->SetName(u8"Camera");
        auto cam = obj->AddComponent<ComponentCamera>();
        cam->SetPositionAndTarget({0, 20, 50}, {0, 10, 0});
    }

    // Ground
    {
        auto obj = Scene::CreateObjectPtr<Object>()->SetName("Ground");
        obj->AddComponent<ComponentModel>("data/Sample/SwordBout/Stage/Stage00.mv1");
        if(auto cmp_mdl = obj->AddComponent<ComponentCollisionModel>())
            cmp_mdl->AttachToModel(true);
    }

    auto p = Player::Create({0, 100, 0});

    //----------------------------------------------------------------------------------
    // 持たせるオブジェクト(Knife)
    //----------------------------------------------------------------------------------
    auto obj = Scene::CreateObjectPtr<Object>()->SetName("Knife");
    obj->SetTranslate({0, 0, 0});
    // オブジェクトにモデルをつける
    if(auto model = obj->AddComponent<ComponentModel>()) {
        model->Load("data/Sample/FPS_Knife/Knife_low.mv1");
        model->SetRotationAxisXYZ({0, 0, 0});
        model->SetScaleAxisXYZ({10.0f, 10.0f, 10.0f});

        // KnifeModelの標準描画をOFFする
        model->SetStatus(Component::StatusBit::NoDraw, true);

        // テクスチャ
        {
            // 刃物部分
            Material mat{};
            mat.albedo_    = std::make_shared<Texture>("data/Sample/FPS_Knife/Knife_low_Iron.001_BaseColor.png");
            mat.normal_    = std::make_shared<Texture>("data/Sample/FPS_Knife/Knife_low_Iron.001_NormalOpenGLl.png");
            mat.roughness_ = std::make_shared<Texture>("data/Sample/FPS_Knife/Knife_low_Iron.001_Roughness.png");
            mat.metalness_ = std::make_shared<Texture>("data/Sample/FPS_Knife/Knife_low_Iron.001_Metallic.png");
            materials_.push_back(mat);
        }
        {
            // 鞘部分
            Material mat{};
            mat.albedo_    = std::make_shared<Texture>("data/Sample/FPS_Knife/Knife_low_Lever.001_BaseColor.png");
            mat.normal_    = std::make_shared<Texture>("data/Sample/FPS_Knife/Knife_low_Lever.001_NormalOpenGLl.png");
            mat.roughness_ = std::make_shared<Texture>("data/Sample/FPS_Knife/Knife_low_Lever.001_Roughness.png");
            mat.metalness_ = std::make_shared<Texture>("data/Sample/FPS_Knife/Knife_low_Lever.001_Metallic.png");
            materials_.push_back(mat);
        }

        // モデルに設定されているテクスチャを上書き
        // model と this(Scene) をこの関数内で使用する
        // modelはshared_ptrとして使うためコピーをとる
        model->SetProc(
            "ModelDraw",
            [model, this]() {
                // この部分をDrawタイミングで使用する
                if(auto model_knife = model->GetModelClass()) {
                    {
                        auto& mat = materials_[0];
                        model_knife->overrideTexture(Model::TextureType::Diffuse, mat.albedo_);
                        model_knife->overrideTexture(Model::TextureType::Albedo, mat.albedo_);
                        model_knife->overrideTexture(Model::TextureType::Normal, mat.normal_);
                        model_knife->overrideTexture(Model::TextureType::Roughness, mat.roughness_);
                        model_knife->overrideTexture(Model::TextureType::Metalness, mat.metalness_);

                        model_knife->renderByMesh(0);   // 金属刃物部分
                    }

                    {
                        auto& mat = materials_[1];
                        model_knife->overrideTexture(Model::TextureType::Diffuse, mat.albedo_);
                        model_knife->overrideTexture(Model::TextureType::Albedo, mat.albedo_);
                        model_knife->overrideTexture(Model::TextureType::Normal, mat.normal_);
                        model_knife->overrideTexture(Model::TextureType::Roughness, mat.roughness_);
                        model_knife->overrideTexture(Model::TextureType::Metalness, mat.metalness_);

                        model_knife->renderByMesh(1);   // 鞘部分
                    }
                }
            },
            ProcTiming::Draw);
    }

    if(auto attach = obj->AddComponent<ComponentAttachModel>()) {
        // mouseの右手にアタッチする
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

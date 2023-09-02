﻿#include "SceneAttachModel.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentAttachModel.h>

// ここにMenu設定を用意します
BP_CLASS_IMPL(SceneAttachModel, u8"AttachModel 使用サンプル");

//! @brief シーン初期化関数を継承します
//! @return シーン初期化が終わったらtrueを返します
bool SceneAttachModel::Init()
{
    //----------------------------------------------------------------------------------
    // カメラ
    //----------------------------------------------------------------------------------
    Scene::CreateObjectPtr<Object>()                       // カメラオブジェクト
        ->SetName("Camera")                                // 名前設定
        ->AddComponent<ComponentCamera>()                  // カメラコンポーネント
        ->SetPositionAndTarget({12, 12, 20}, {0, 6, 0});   // ポジションと注視点

    //----------------------------------------------------------------------------------
    // 本体キャラの作成
    //----------------------------------------------------------------------------------
    auto mouse = Scene::CreateObjectPtr<Object>()->SetName("Mouse");

    // 本体キャラにモデルをつける
    if(auto model = mouse->AddComponent<ComponentModel>()) {
        model->Load("data/Sample/Player/model.mv1");
        model->SetRotationAxisXYZ({0, 180, 0});
        model->SetAnimation({
            {"walk", "data/Sample/Player/Anim/Walk.mv1", 1, 1.0f}, // idle
            {"jump", "data/Sample/Player/Anim/Jump.mv1", 1, 1.0f}  // jump
        });
        model->PlayAnimation("walk", true);
    }

    //----------------------------------------------------------------------------------
    // 持たせるオブジェクト(Knife)
    //----------------------------------------------------------------------------------
    auto obj = Scene::CreateObjectPtr<Object>()->SetName("Knife");
    obj->SetTranslate({0, 10, 10});
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
        attach->SetAttachObject(mouse, "mixamorig:RightHand");
        // Knife回転量
        attach->SetAttachRotate({-12.5, 75, 180});
        // Knifeオフセット
        attach->SetAttachOffset({15, 10, 2});
    }

    return true;
}

//! @brief シーン更新関数。ディスプレイリフレッシュレートに合わせて実行されます
void SceneAttachModel::Update()
{
}

void SceneAttachModel::Exit()
{
}

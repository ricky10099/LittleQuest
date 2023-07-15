//---------------------------------------------------------------------------
//! @file   Stage01.h
//! @brief  Stage01シーン
//---------------------------------------------------------------------------
#pragma once

#include <System/Scene.h>

//===========================================================================
//! Stage01シーン
//===========================================================================
class Stage01 : public Scene::Base
{
public:
    BP_CLASS_TYPE(Stage01, Scene::Base)

    bool Init() override;     //!< 初期化
    void Update() override;   //!< 更新
    void Draw() override;     //!< 描画
    void Exit() override;     //!< 終了
    void GUI() override;      //!< GUI表示

private:
    struct Material
    {
        std::shared_ptr<Texture> albedo_;
        std::shared_ptr<Texture> normal_;
        std::shared_ptr<Texture> roughness_;
        std::shared_ptr<Texture> metalness_;
    };

    std::vector<Material> materials_;
};

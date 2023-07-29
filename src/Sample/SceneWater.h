﻿//---------//---------------------------------------------------------------------------
//---------//---------------------------------------------------------------------------
//! @file   SceneWater.h
//! @brief  水面サンプルシーン
//---------------------------------------------------------------------------
#pragma once

#include <System/Scene.h>

class Model;

//===========================================================================
//! 水面サンプルシーン
//===========================================================================
class SceneWater final : public Scene::Base
{
public:
    BP_CLASS_TYPE(SceneWater, Scene::Base);

    //----------------------------------------------------------
    //! @name   シーン関数
    //----------------------------------------------------------
    //@{

    virtual bool Init() override;     //!< 初期化
    virtual void Update() override;   //!< 更新
    virtual void Draw() override;     //!< 描画
    virtual void Exit() override;     //!< 終了
    virtual void GUI() override;      //!< GUI表示

    //@}

private:
    std::shared_ptr<Model> model_;   //!< 3Dモデル
};

﻿//---------------------------------------------------------------------------
//! @file   SceneEaseCurve.h
//! @brief  Ease補間サンプルシーン
//---------------------------------------------------------------------------
#pragma once

#include <System/Scene.h>

//===========================================================================
//! Ease補間サンプルシーン
//===========================================================================
class SceneEaseCurve final : public Scene::Base
{
public:
    BP_CLASS_TYPE(SceneEaseCurve, Scene::Base)

    bool Init() override;     //!< 初期化
    void Update() override;   //!< 更新
    void Draw() override;     //!< 描画
    void Exit() override;     //!< 終了
    void GUI() override;      //!< GUI表示

private:
    f32 t_ = 0.0f;   //!< 経過時間 (単位:秒)
};

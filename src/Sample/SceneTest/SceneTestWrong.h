﻿#pragma once

#include <System/Scene.h>

//namespace WRONG
//{
//! @brief シーン
//! @detail Scene::Baseより継承させ Scene○○と名前をつけておく。
//! @detail 今回はObjectを使用する説明のためSceneTestWrongと名前をつけています
//! 本来シーンは、タイトル、ゲーム中、ゲームオーバーなどのシーンを用意し、切り替えて使えます
class SceneTestWrong : public Scene::Base
{
public:
    // 継承している形をここでも書いておく(プログラム的に使用する)
    BP_CLASS_TYPE(SceneTestWrong, Scene::Base);

    //! @brief シーン初期化関数を継承します
    //! @return シーン初期化が終わったらtrueを返します
    bool Init() override;

    //! @brief シーン更新関数。ディスプレイリフレッシュレートに合わせて実行されます
    //! @param delta 1秒をベースとした1フレームの数値
    //! @detial deltaは、リフレッシュレートが違うと速度が変わってしまう部分を吸収するためにある
    void Update() override;
};

//}	// namespace WRONG

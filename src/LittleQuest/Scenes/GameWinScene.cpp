#include "GameWinScene.h"

#include "GameTitleScene.h"

namespace LittleQuest
{
BP_CLASS_IMPL(GameWinScene, u8"LittleQuest/GameWinScene");

bool GameWinScene::Init()
{
    image = LoadGraph("data/LittleQuest/Image/Win.png");
    return true;
}

void GameWinScene::Update()
{
    //// 3秒たったらタイトルへ
    // if (Scene::GetTime() >= 3.0f)
    //     Scene::Change(Scene::GetScene<GameTitleScene>());
    if(IsKeyOn(KEY_INPUT_RETURN))
        Scene::Change(Scene::GetScene<GameTitleScene>());
}

void GameWinScene::Draw()
{
    // とりあえずTitleという文字を表示しておく
    // DrawFormatString(100, 50, GetColor(255, 0, 0), "GameWin");
    DrawGraph(0, 0, image, TRUE);
}

void GameWinScene::Exit()
{
    // タイトル終了時に行いたいことは今はない
}

void GameWinScene::GUI()
{
}

}   // namespace LittleQuest

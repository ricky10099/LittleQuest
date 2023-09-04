#include "GameOverScene.h"

#include "GameTitleScene.h"

namespace LittleQuest
{
BP_CLASS_IMPL(GameOverScene, u8"LittleQuest/GameOverScene");

bool GameOverScene::Init()
{
    image = LoadGraph("data/LittleQuest/Image/GameOver.png");
    return true;
}

void GameOverScene::Update()
{
    //// 3秒たったらタイトルへ
    // if (Scene::GetTime() >= 3.0f)
    //     Scene::Change(Scene::GetScene<GameTitleScene>());
    if(IsKeyOn(KEY_INPUT_RETURN))
        Scene::Change(Scene::GetScene<GameTitleScene>());
}

void GameOverScene::Draw()
{
    // とりあえずTitleという文字を表示しておく
    // DrawFormatString(100, 50, GetColor(255, 0, 0), "GameOver");
    DrawGraph(0, 0, image, TRUE);
}

void GameOverScene::Exit()
{
    // タイトル終了時に行いたいことは今はない
}

void GameOverScene::GUI()
{
}

}   // namespace LittleQuest

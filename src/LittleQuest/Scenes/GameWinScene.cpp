#include "GameWinScene.h"

#include "GameTitleScene.h"

namespace LittleQuest {
BP_CLASS_IMPL(GameWinScene, u8"LittleQuest/GameWinScene");

bool GameWinScene::Init() {
    image = LoadGraph("data/LittleQuest/Image/Win.png");
    return true;
}

void GameWinScene::Update() {
    if(IsKeyOn(KEY_INPUT_RETURN))
        Scene::Change(Scene::GetScene<GameTitleScene>());
}

void GameWinScene::Draw() {
    DrawGraph(0, 0, image, TRUE);
    DrawFormatString(500, 500, GetColor(0, 0, 0), "Press Enter key back to Title");
}

void GameWinScene::Exit() {
    // タイトル終了時に行いたいことは今はない
}

void GameWinScene::GUI() {}

}    // namespace LittleQuest

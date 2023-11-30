#include "GameOverScene.h"

#include "GameTitleScene.h"

namespace LittleQuest {
    BP_CLASS_IMPL(GameOverScene, u8"LittleQuest/GameOverScene");

    bool GameOverScene::Init() {
        image = LoadGraph("data/LittleQuest/Image/GameOver.png");
        return true;
    }

    void GameOverScene::Update() {
        if (IsKeyOn(KEY_INPUT_RETURN)) Scene::Change(Scene::GetScene<GameTitleScene>());
    }

    void GameOverScene::Draw() {
        DrawGraph(0, 0, image, TRUE);
        DrawFormatString(500, 500, GetColor(0, 0, 0), "Press Enter key back to Title");
    }

    void GameOverScene::Exit() {
        // タイトル終了時に行いたいことは今はない
    }

    void GameOverScene::GUI() {}

}    // namespace LittleQuest

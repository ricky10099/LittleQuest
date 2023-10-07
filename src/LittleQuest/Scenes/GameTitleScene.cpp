#include "GameTitleScene.h"

#include "Stage01.h"

namespace LittleQuest {
    BP_CLASS_IMPL(GameTitleScene, u8"LittleQuest/GameTitleScene");

    bool GameTitleScene::Init() {
        Scene::CreateObjectPtr<Object>()
            ->SetName(u8"Camera")
            ->AddComponent<ComponentCamera>();

        image = LoadGraph("data/LittleQuest/Image/title.png");

        return true;
    }

    void GameTitleScene::Update() {
        if (IsKeyOn(KEY_INPUT_RETURN))
            Scene::Change(Scene::GetScene<Stage01>());
    }

    void GameTitleScene::Draw() {
        DrawGraph(0, 0, image, TRUE);
        DrawFormatString(500, 500, GetColor(0, 0, 0),
                         "Press Enter key to start");
    }

    void GameTitleScene::Exit() {
        // タイトル終了時に行いたいことは今はない
    }

    void GameTitleScene::GUI() {}

}    // namespace LittleQuest

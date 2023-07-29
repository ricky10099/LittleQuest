#include "OverScene.h"
#include "../Title/TitleScene.h"

namespace Sample
{
namespace GameSample
{
BP_CLASS_IMPL(OverScene, u8"GameSample/OverScene");

bool OverScene::Init()
{
    return true;
}

void OverScene::Update()
{
    // 3秒たったらタイトルへ
    if(Scene::GetTime() >= 3.0f)
        Scene::Change(Scene::GetScene<TitleScene>());
}

void OverScene::Draw()
{
    // とりあえずTitleという文字を表示しておく
    DrawFormatString(100, 50, GetColor(255, 0, 0), "GameOver");
}

void OverScene::Exit()
{
    // タイトル終了時に行いたいことは今はない
}

void OverScene::GUI()
{
}

}   // namespace GameSample
}   // namespace Sample

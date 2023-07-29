#include <System/Scene.h>

namespace Tutorial
{
//-------------------------------------------------------
class Tutorial_05 : public Scene::Base
{
public:
    BP_CLASS_TYPE(Tutorial_05, Scene::Base);

    //! @brief 初期化
    //! @return 初期化が終わったか
    bool Init() override;

    //! @brief 更新
    //! @param delta 更新周期
    void Update() override;

    //! @brief 描画
    void Draw() override;

    //! @brief デバッグ表示用GUI
    void GUI() override;

    //! @brief 終了
    void Exit() override;

private:
};

//------------------------------------------------------------------
BP_CLASS_IMPL(Tutorial_05, u8"(5)Tutorial オブジェクト使用方法");

bool Tutorial_05::Init()
{
    Scene::CreateObjectPtr<Object>()    //< Object作成
        ->SetName(u8"オブジェクト零")   //< 名前設定
        ->SetTranslate({0, 0, 0});      //< 位置設定

    Scene::CreateObjectPtr<Object>()    //< Object作成
        ->SetName(u8"オブジェクト壱")   //< 名前設定
        ->SetTranslate({-2, 0, 0});     //< 位置設定

    Scene::CreateObjectPtr<Object>()    //< Object作成
        ->SetName(u8"オブジェクト弐")   //< 名前設定
        ->SetTranslate({2, 0, 0});      //< 位置設定

    Scene::CreateObjectPtr<Object>()         //< Object作成
        ->SetName(u8"標準カメラ")            //< 名前設定
        ->AddComponent<ComponentCamera>();   //< 標準カメラ

    return true;
}

void Tutorial_05::Update()
{
    // オブジェクトを取得
    if(auto obj = Scene::GetObjectPtr<Object>(u8"オブジェクト壱")) {
        // そのオブジェクトをY軸回転で、1度回す
        obj->AddRotationAxisXYZ({0, 1, 0});
        // そのオブジェクトをZ軸方向に、0.01動かす (グローバルのZ軸)
        obj->AddTranslate({0, 0, 0.01f});   // グローバル方向に移動
    }

    if(auto obj = Scene::GetObjectPtr<Object>(u8"オブジェクト弐")) {
        // そのオブジェクトをY軸回転で、1度回す
        obj->AddRotationAxisXYZ({0, -1, 0});
        // そのオブジェクトをZ軸方向に、0.01動かす (自分の向きのZ軸)
        obj->AddTranslate({0, 0, 0.01f}, true);   // ローカル方向に移動
    }
}

void Tutorial_05::Draw()
{
    DrawFormatString(0, 0, GetColor(0, 255, 255), "Objectテスト\n");
}

void Tutorial_05::GUI()
{
    // HowToUseというGUIWindowを以下の内容で表示する
    ImGui::Begin("HowToUse");
    {
        ImGui::Separator();
        ImGui::Text("Objectとは?\n "   //
                    u8"\n"             //
                    u8"シーン出てくる物体です。\n"
                    u8"ComponentTransformをもっており、\n"
                    u8"これにより、位置や回転姿勢、\n"
                    u8"大きさを変更できます\n"
                    u8"\n"
                    u8"シーン内では、\n"
                    u8"Scene::CreateObjectPtr<型>()\n"
                    u8"で作成することができます。\n"
                    u8"\n"
                    u8"また\n"
                    u8"auto obj = Scene::GetObjectPtr<型>()\n"
                    u8"で取得することができます。(先に見つかったものが取得される)\n"
                    u8"auto obj = Scene::GetObjectPtr<型>(名前)\n"
                    u8"で取得すると名前をつけたそのものを取得できます\n"
                    u8"\n"
                    u8"同じ型のオブジェクトが複数ある場合、\n"
                    u8"auto objs = Scene::GetObjectsPtr<型>()\n"
                    u8"で複数同時に取得することができます。\n"
                    u8"\n"
                    u8"for(auto obj : objs) {}\n"
                    u8"で使用してください\n");
    }
    ImGui::End();
}

void Tutorial_05::Exit()
{
}

}   // namespace Tutorial

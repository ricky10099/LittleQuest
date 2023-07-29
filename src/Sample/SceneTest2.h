#pragma once

#include <System/Scene.h>

namespace Sample
{
class SceneTest2 : public Scene::Base
{
public:
    BP_CLASS_TYPE(SceneTest2, Scene::Base);

    bool Init() override;
    void Update() override;
    void Draw() override;
    void Exit() override;
    void GUI() override;
};
}   // namespace Sample

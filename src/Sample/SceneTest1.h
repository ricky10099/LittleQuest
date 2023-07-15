#pragma once

#include <System/Scene.h>

namespace Sample
{
class SceneTest1 : public Scene::Base
{
public:
    BP_CLASS_TYPE(SceneTest1, Scene::Base);

    bool Init() override;
    void Update() override;
    void Draw() override;
    void Exit() override;
    void GUI() override;

    void InitSerialize() override;
};
}   // namespace Sample

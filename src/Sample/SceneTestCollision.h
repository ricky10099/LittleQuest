#pragma once

#include <System/Scene.h>

class SceneTestCollision : public Scene::Base
{
public:
    BP_CLASS_TYPE(SceneTestCollision, Scene::Base);

    bool Init() override;
    void Update() override;
    void Draw() override;
    void Exit() override;
    void GUI() override;
};

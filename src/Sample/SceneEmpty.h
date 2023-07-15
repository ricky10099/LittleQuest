#pragma once

#include <System/Scene.h>

class SceneEmpty : public Scene::Base
{
public:
    BP_CLASS_TYPE(SceneEmpty, Scene::Base);

    bool Init() override;

    void Update() override;
};

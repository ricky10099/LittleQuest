#include "Camera.h"

#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {
BP_OBJECT_IMPL(Camera, u8"LittleQuest/Camera");

CameraPtr Camera::Create(ObjectPtr obj) {
    auto camPtr       = Scene::CreateObjectDelayInitialize<Camera>();
    camPtr->targetPtr = obj;
    return camPtr;
}

bool Camera::Init() {
    bool ret = Super::Init();
    if(!ret) {
        return false;
    }

    auto camera = AddComponent<ComponentCamera>();
    camera->SetPositionAndTarget({0, 0, -1}, {0, 0, 0});

    auto col = AddComponent<ComponentCollisionSphere>();
    col->SetRadius(1.0f);
    col->SetMass(0.0f);
    col->SetCollisionGroup(ComponentCollision::CollisionGroup::CAMERA);
    col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::GROUND | (u32)ComponentCollision::CollisionGroup::WALL);

    pStpringArm = AddComponent<ComponentSpringArm>();
    pStpringArm.lock()->SetSpringArmObject(targetPtr.lock());
    pStpringArm.lock()->SetSpringArmVector({0, 0, 0});
    pStpringArm.lock()->SetSpringArmLength(50);
    pStpringArm.lock()->SetSpringArmOffset({0, 10, 0});

    return true;
}

void Camera::Update() {
    rot += {-GetMouseMoveY(), GetMouseMoveX(), 0};
    rot.x = max(min(rot.x, 40.0f), -70.0f);
    pStpringArm.lock()->SetSpringArmRotate(rot);
}

void Camera::SetCameraLength(float length) {
    pStpringArm.lock()->SetSpringArmLength(length);
}
}    // namespace LittleQuest

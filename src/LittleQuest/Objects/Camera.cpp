#include "Camera.h"

#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {
BP_OBJECT_IMPL(Camera, "LittleQuest/Camera");

CameraPtr Camera::Create(ObjectPtr obj) {
    auto pCamera       = Scene::CreateObjectDelayInitialize<Camera>();
    pCamera->m_pTarget = obj;
    return pCamera;
}

bool Camera::Init() {
    m_pCamera = AddComponent<ComponentCamera>();
    m_pCamera.lock()->SetPositionAndTarget({0, 0, -1}, {0, 0, 0});

    auto col = AddComponent<ComponentCollisionSphere>();
    col->SetRadius(1.0f);
    col->SetMass(0.0f);
    col->SetCollisionGroup(ComponentCollision::CollisionGroup::CAMERA);
    col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::GROUND | (u32)ComponentCollision::CollisionGroup::WALL);

    m_pSpringArm = AddComponent<ComponentSpringArm>();
    m_pSpringArm.lock()->SetSpringArmObject(m_pTarget.lock());
    m_pSpringArm.lock()->SetSpringArmRotate(m_rot);
    m_pSpringArm.lock()->SetSpringArmLength(50);
    m_pSpringArm.lock()->SetSpringArmOffset({0, 5, 0});

    return Super::Init();
}

void Camera::Update() {
    if(m_pCamera.lock()->GetCurrentCamera().lock() == m_pCamera.lock()) {
        DINPUT_JOYSTATE DInputState;
        switch(GetJoypadType(DX_INPUT_PAD1)) {
        case DX_PADTYPE_DUAL_SENSE:
            GetJoypadDirectInputState(DX_INPUT_PAD1, &DInputState);
            m_rot += {-DInputState.Rz * 0.001f, DInputState.Z * 0.001f, 0};
            break;
        default:
            break;
        }
        m_rot += {-GetMouseMoveY() * 0.1f, GetMouseMoveX() * 0.1f, 0};
        m_rot.x = max(min(m_rot.x, 40.0f), -70.0f);
        m_pSpringArm.lock()->SetSpringArmRotate(m_rot);
    }
}

const float3 Camera::CameraForward() {
    return -this->GetMatrix().axisZ();
}

void Camera::SetCameraLookTarget(ObjectWeakPtr pTarget) {
    m_pSpringArm.lock()->SetSpringArmObject(pTarget.lock());
}

void Camera::SetCameraLength(float length) {
    m_pSpringArm.lock()->SetSpringArmLength(length);
}

void Camera::SetCurrentCamera() {
    m_pCamera.lock()->SetCurrentCamera();
}
}    // namespace LittleQuest

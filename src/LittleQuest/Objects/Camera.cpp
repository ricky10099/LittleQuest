#include "Camera.h"

#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {
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

    //    m_pCorrectionLine = AddComponent<ComponentCollisionLine>();
    //    m_pCorrectionLine.lock()->SetTranslate({0, 0, 0});
    //    m_pCorrectionLine.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::CAMERA);
    //    m_pCorrectionLine.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ETC);
    //    m_pCorrectionLine.lock()->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::ETC);
    //#if _DEBUG
    //    m_pCorrectionLine.lock()->ShowInGame(true);
    //#endif

    return Super::Init();
}

void Camera::Update() {
    if(m_pCamera.lock()->GetCurrentCamera().lock() == m_pCamera.lock() && !m_isLockOn) {
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

        if(m_pTarget.lock() != nullptr) {
            float3 objPos = m_pSpringArm.lock()->GetSpringArmObject().lock()->GetTranslate();
            float3 dir    = {1, objPos.y - this->GetTranslate().y, 1};
            float3 dir2   = dir + float3{0, 100, 0};
            //m_pCorrectionLine.lock()->SetLine({0, 0, 0},dir);
            //m_pSpringArm.lock()->GetSpringArmObject().lock()->GetTranslate() - this->GetTranslate());
        }
    } else if(m_isLockOn) {
    }
}

void Camera::LateDraw() {
#if _DEBUG
    if(Scene::IsEdit()) {
        //float3 dir =
        //    float3{0, 0, 0} - (this->GetTranslate() - m_pSpringArm.lock()->GetSpringArmObject().lock()->GetTranslate());
        //printfDx("camera player vector3: x: %f y: %f z: %f\n", (float)dir.x, (float)dir.y, (float)dir.z);
        //printfDx("camera player vector3: x: %f y: %f z: %f\n", (float)dir.x, (float)dir.y, (float)dir.z);
        printfDx("camera length: %f\n", m_pSpringArm.lock()->GetSpringArmLength());
    }
#endif
}

void Camera::OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hitInfo) {
    Super::OnHit(hitInfo);
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

float3 Camera::SetLockOnTarget(ObjectWeakPtr pTarget) {
    m_isLockOn = true;
    float3 dir = -(pTarget.lock()->GetTranslate() - m_pTarget.lock()->GetTranslate());
    dir += m_pTarget.lock()->GetTranslate();
    this->SetTranslate(dir);
}
}    // namespace LittleQuest

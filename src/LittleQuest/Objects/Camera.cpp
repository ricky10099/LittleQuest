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

    m_pCollision = AddComponent<ComponentCollisionSphere>();
    m_pCollision.lock()->SetRadius(1.0f);
    m_pCollision.lock()->SetMass(0.0f);
    m_pCollision.lock()->SetCollisionGroup(ComponentCollision::CollisionGroup::CAMERA);
    m_pCollision.lock()->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::GROUND |
                                              (u32)ComponentCollision::CollisionGroup::WALL);
    m_pCollision.lock()->SetName(COLLISION_NAME);

    m_pSpringArm = AddComponent<ComponentSpringArm>();
    m_pSpringArm.lock()->SetSpringArmObject(m_pTarget.lock());
    m_pSpringArm.lock()->SetSpringArmRotate(m_rot);
    m_pSpringArm.lock()->SetSpringArmLength(50);
    m_pSpringArm.lock()->SetSpringArmOffset({0, 10, 0});
    m_pSpringArm.lock()->SetSpringArmVector({1, 1, 1});

    srand(static_cast<unsigned int>(time(nullptr)));

    return Super::Init();
}

void Camera::Update() {
    m_pCollision.lock()->SetTranslate(m_pCamera.lock()->GetLocalPosition());

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

    } else if(m_isLockOn) {
        float3 dir   = (m_pLockOnTarget.lock()->GetTranslate() - m_pTarget.lock()->GetTranslate());
        float  dx    = m_pLockOnTarget.lock()->GetTranslate().x - m_pTarget.lock()->GetTranslate().x;
        float  dz    = m_pLockOnTarget.lock()->GetTranslate().z - m_pTarget.lock()->GetTranslate().z;
        float  angle = atan2(dz, dx);
        angle *= RadToDeg;

        float3 new_rot = {-10, -100 - angle, m_rot.z};
        m_pSpringArm.lock()->SetSpringArmRotate(new_rot);
        m_pSpringArm.lock()->SetSpringArmOffset({5, 5, 0});
        m_pSpringArm.lock()->SetSpringArmLength(30);

        printfDx("angle: %f\n", angle);
        printfDx("y - angle: %f\n", m_rot.y - angle);
    }

#ifdef _DEBUG
    static float fff = 1;
    if(IsKeyOn(KEY_INPUT_P)) {
        SetCameraShake(600, 100);
    }

    /* if (IsKeyOn(KEY_INPUT_PERIOD)) {
        fff++;
    }
    if (IsKeyOn(KEY_INPUT_COMMA)) {
        fff--;
    }
    m_pSpringArm.lock()->SetSpringArmVector({1, fff, 1});*/

#endif    //  _DEBUG \
          //         \
          //
    ShakeCamera();
    //    this->SetTranslate({10, 100, 10});
}

void Camera::LateDraw() {
#if _DEBUG
    if(Scene::IsEdit()) {
        //float3 dir =
        //    float3{0, 0, 0} - (this->GetTranslate() - m_pSpringArm.lock()->GetSpringArmObject().lock()->GetTranslate());
        //printfDx("camera player vector3: x: %f y: %f z: %f\n", (float)dir.x, (float)dir.y, (float)dir.z);
        //printfDx("camera player vector3: x: %f y: %f z: %f\n", (float)dir.x, (float)dir.y, (float)dir.z);
        printfDx("player pos: x:%f y:%f z:%f\n", (float)m_pTarget.lock()->GetTranslate().x,
                 (float)m_pTarget.lock()->GetTranslate().y, (float)m_pTarget.lock()->GetTranslate().z);
        if(m_pLockOnTarget.lock()) {
            printfDx("target pos: x:%f y:%f z:%f\n", (float)m_pLockOnTarget.lock()->GetTranslate().x,
                     (float)m_pLockOnTarget.lock()->GetTranslate().y, (float)m_pLockOnTarget.lock()->GetTranslate().z);
            float3 dis = m_pLockOnTarget.lock()->GetTranslate() - m_pTarget.lock()->GetTranslate();
            printfDx("target pos: x:%f y:%f z:%f\n", (float)dis.x, (float)dis.y, (float)dis.z);
        }
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
    if(m_pSpringArm.lock()) {
        m_pSpringArm.lock()->SetSpringArmLength(length);
    }
}

void Camera::SetCurrentCamera() {
    m_pCamera.lock()->SetCurrentCamera();
}

float3 Camera::SetLockOnTarget(ObjectWeakPtr pTarget, bool isLockOn) {
    m_isLockOn       = isLockOn;
    m_pLockOnTarget  = pTarget;
    m_lockOnPosition = pTarget.lock()->GetTranslate();
    m_pSpringArm.lock()->SetSpringArmOffset({0, 5, 0});

    return pTarget.lock()->GetTranslate();
}

void Camera::SetCameraShake(float duration, float magnitude) {
    shakeDuration     = duration;
    shakeMagnitude    = magnitude;
    shakeTimer        = duration;
    originalCameraPos = m_pCamera.lock()->GetLocalPosition();
}

void Camera::ShakeCamera() {
    if(shakeTimer > 0.0f) {
        shakeTimer -= GetDeltaTime60();    // Assuming GetDeltaTime() gives the time elapsed since the last frame

        // Calculate random offset for shake
        float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * shakeMagnitude;
        float offsetY = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * shakeMagnitude;
        //float offsetZ = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * shakeMagnitude;

        // Apply offset to camera position
        float3 shakePos = originalCameraPos + float3{offsetX, offsetY, 0};
        /*m_pCamera.lock()->SetPositionAndTarget(shakePos, originalCameraPos);*/
        m_pCamera.lock()->SetPositionAndTarget(shakePos, {0, 0, 0});
        //this->SetTranslate({10, 100, 10});

        // Gradually decrease shake magnitude
        shakeMagnitude *= 0.9f;

        // Check if the shake duration has ended
        if(shakeTimer <= 0.0f) {
            // Reset the camera to its original position
            m_pSpringArm.lock()->SetSpringArmVector(originalCameraPos);
        }
    }
}
}    // namespace LittleQuest

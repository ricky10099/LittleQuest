#include "Camera.h"

#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {
    // OBJECT名前
    BP_OBJECT_IMPL(Camera, u8"LittleQuest/Camera");

    //! カメラ初期化
    bool Camera::Init() {
        bool ret = __super::Init();
        if (!ret) return false;

        // カメラの設定
        auto camera = AddComponent<ComponentCamera>();
        camera->SetPositionAndTarget({0, 0, -1}, {0, 0, 0});

        // カメラの当たり判定
        auto col = AddComponent<ComponentCollisionSphere>();
        col->SetRadius(1.0f);
        col->SetMass(0.0f);
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::CAMERA);
        col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::GROUND
                                  | (u32)ComponentCollision::CollisionGroup::WALL);

        // スプリングアーム設定
        auto spring_arm = AddComponent<ComponentSpringArm>();

        spring_arm->SetSpringArmObject(target_obj_.lock());

        spring_arm->SetSpringArmVector({0, 0, 0});
        spring_arm->SetSpringArmLength(50);
        spring_arm->SetSpringArmOffset({0, 10, 0});

        return true;
    }

    // カメラ作成関数
    CameraPtr Camera::Create(ObjectPtr obj) {
        auto camobj         = Scene::CreateObjectDelayInitialize<Camera>();
        camobj->target_obj_ = obj;
        return camobj;
    }

    // マウスで動作させるための設定
    // #ifdef USE_MOUSE_CAMERA
    void Camera::Update() {
        auto spring_arm = GetComponent<ComponentSpringArm>();

        // SprintArmの回転にマウスの移動量を足しこむ
        rot_ += {-GetMouseMoveY(), GetMouseMoveX(), 0};

        if (rot_.x > 70) rot_.x = 70;
        if (rot_.x < -70) rot_.x = -70;
        // 足しこんだ回転を利用する
        spring_arm->SetSpringArmRotate(rot_);
    }
    // #endif
}    // namespace LittleQuest

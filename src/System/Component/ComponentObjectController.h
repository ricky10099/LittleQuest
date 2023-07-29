﻿#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>

class ComponentObjectController : public Component
{
public:
    BP_COMPONENT_TYPE(ComponentObjectController, Component);

    void Init() override;

    void Update() override;

    void [[deprecated("SetSpeed()は古い形式です。SetMoveSpeed()を使用してください")]] SetSpeed(const float speed)
    {
        return SetMoveSpeed(speed);
    }
    void SetMoveSpeed(const float speed);
    void SetRotateSpeed(const float speed);

    const float [[deprecated("GetSpeed()は古い形式です。GetMoveSpeed()を使用してください")]] GetSpeed() const
    {
        return GetMoveSpeed();
    }
    const float GetMoveSpeed() const;
    const float GetRotateSpeed() const;

    void SetKeys(int up, int down, int left, int right);

    void SetCameraKeys(int up, int down, int left, int right);

    void SetTarget(ObjectPtr target);

    void SetCameraRotate(float3 rot);

    void SetUseMouse(bool use = true);
    bool IsUseMouse();

    void GUI() override;

private:
    float move_speed_ = 1.0f;
    float rot_speed_  = 5.0f;

    int key_up_    = KEY_INPUT_W;
    int key_down_  = KEY_INPUT_S;
    int key_left_  = KEY_INPUT_A;
    int key_right_ = KEY_INPUT_D;

    float cam_speed_ = 1.0f;
    int   cam_up_    = KEY_INPUT_UP;
    int   cam_down_  = KEY_INPUT_DOWN;
    int   cam_left_  = KEY_INPUT_LEFT;
    int   cam_right_ = KEY_INPUT_RIGHT;

    float cam_rx_ = 0;
    float cam_ry_ = 0;

    bool  use_mouse_        = false;
    float mouse_up_down_    = 10.0f;
    float mouse_left_right_ = 10.0f;

    float limit_cam_up_   = 5.0f;     //!< 上を眺める
    float limit_cam_down_ = -45.0f;   //!< 下を見る

    ObjectWeakPtr target_;
    float         target_cam_side_speed_ = 3.0f;    //!< ロックオン時のカーソル移動速度
    float         target_cam_up_down     = 10.0f;   //!< ターゲットを見る際の上下固定

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    //! @brief セーブ
    // @param arc アーカイバ
    // @param ver バージョン
    CEREAL_SAVELOAD(arc, ver)
    {
        arc(cereal::make_nvp("speed_", move_speed_),
            CEREAL_NVP(rot_speed_),

            CEREAL_NVP(key_up_),
            CEREAL_NVP(key_down_),
            CEREAL_NVP(key_left_),
            CEREAL_NVP(key_right_),

            CEREAL_NVP(cam_speed_),
            CEREAL_NVP(cam_up_),
            CEREAL_NVP(cam_down_),
            CEREAL_NVP(cam_left_),
            CEREAL_NVP(cam_right_),

            CEREAL_NVP(cam_rx_),
            CEREAL_NVP(cam_ry_),

            CEREAL_NVP(limit_cam_up_),
            CEREAL_NVP(limit_cam_down_),

            CEREAL_NVP(target_),
            CEREAL_NVP(target_cam_side_speed_),
            CEREAL_NVP(target_cam_up_down));

        if(ver >= 3) {
            arc(CEREAL_NVP(use_mouse_),   //
                CEREAL_NVP(mouse_up_down_),
                CEREAL_NVP(mouse_left_right_));
        }

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};

CEREAL_CLASS_VERSION(ComponentObjectController, 3);

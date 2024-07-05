#include <System/Scene.h>

#include <System/Component/ComponentSpringArm.h>

#ifndef CAMERA_OBJ_HEADER
#    define CAMERA_OBJ_HEADER
namespace LittleQuest {
USING_PTR(Camera);
//////////////////////////////////////////////////////////////
//! @brief カメラクラス
//////////////////////////////////////////////////////////////
class Camera: public Object {
    BP_OBJECT_DECL(Camera, u8"LittleQuest/Camera");

   public:
    //------------------------------------------------------------
    //! @brief カメラを生成します。
    //------------------------------------------------------------
    static CameraPtr Create(ObjectPtr obj);

    //------------------------------------------------------------
    //! @brief 初期化処理を行います。
    //!
    //! @retval true 初期化成功
    //! @retval false 初期化失敗
    //------------------------------------------------------------
    bool Init() override;
    //------------------------------------------------------------
    //! @brief 更新処理を行います。
    //------------------------------------------------------------
    void Update() override;

    void LateDraw() override;

    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;

    //------------------------------------------------------------
    //! @brief カメラの前ベクトルを取得します。
    //!
    //! @return カメラの前ベクトル
    //------------------------------------------------------------
    const float3 CameraForward();

    //------------------------------------------------------------
    //! @brief カメラの距離を設定します。
    //!
    //! @param length カメラの距離
    //------------------------------------------------------------
    void SetCameraLength(float length);

    //------------------------------------------------------------
    //! @brief カメラの目標を設定します。
    //!
    //! @param pTarget 目標ポインター
    //------------------------------------------------------------
    void SetCameraLookTarget(ObjectWeakPtr pTarget);

    //------------------------------------------------------------
    //! @brief メインカメラに設定します。
    //------------------------------------------------------------
    void SetCurrentCamera();

    inline void SetCameraPosition(float3 pos) {
        this->SetTranslate(pos);
    }

    inline void SetCameraPositionAndTarget(float3 position, float3 target) {
        m_pCamera.lock()->SetPositionAndTarget(position, target);
    }

    float3 SetLockOnTarget(ObjectWeakPtr pTarget, bool isLockOn);

    void SetCameraShake(float duration, float magnitude);

    void ShakeCamera();

   private:
    const std::string_view COLLISION_NAME = "CollisionSphere";

    //! カメラの回転
    float3 m_rot{-20, -90, 0};

    float3        m_lockOnPosition;
    //! カメラの目標
    ObjectWeakPtr m_pTarget;

    ObjectWeakPtr m_pLockOnTarget;

    float  shakeDuration  = 0.0f;
    float  shakeMagnitude = 0.0f;
    float  shakeTimer     = 0.0f;
    float3 originalCameraPos;

    //! カメラコンポーネント
    std::weak_ptr<ComponentCamera>    m_pCamera;
    //! スプリングアームコンポーネント
    std::weak_ptr<ComponentSpringArm> m_pSpringArm;

    std::weak_ptr<ComponentCollisionSphere> m_pCollision;

    bool m_isLockOn = false;
    bool m_isOnHit  = false;
    //std::weak_ptr<ComponentCollisionLine> m_pCorrectionLine;
};
}    // namespace LittleQuest
#endif

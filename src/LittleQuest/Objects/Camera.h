#include <System/Scene.h>

#include <System/Component/ComponentSpringArm.h>

#ifndef CAMERA_OBJ_HEADER
#    define CAMERA_OBJ_HEADER
namespace LittleQuest {
USING_PTR(Camera);
class Camera: public Object {
    BP_OBJECT_DECL(Camera, u8"LittleQuest/Camera");

   public:
    static CameraPtr Create(ObjectPtr obj);

    bool Init() override;
    void Update() override;

    const float3 CameraForward();

    void SetCameraLength(float length);
    void SetCameraLookTarget(ObjectWeakPtr pTarget);
    void SetCurrentCamera();

   private:
    float3        m_rot{-20, -90, 0};
    ObjectWeakPtr m_pTarget;

    std::weak_ptr<ComponentCamera>    m_pCamera;
    std::weak_ptr<ComponentSpringArm> m_pSpringArm;
};
}    // namespace LittleQuest
#endif

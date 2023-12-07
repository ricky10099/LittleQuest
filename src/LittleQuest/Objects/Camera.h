#include <System/Scene.h>

#include <System/Component/ComponentSpringArm.h>

namespace LittleQuest {

USING_PTR(Camera);

class Camera: public Object {
    BP_OBJECT_TYPE(Camera, Object);

   public:
    static CameraPtr Create(ObjectPtr obj);
    bool             Init() override;
    void             Update() override;
    void             SetCameraLength(float length);

   private:
    float3        rot{-20, 0, 0};
    ObjectWeakPtr targetPtr;

    std::weak_ptr<ComponentSpringArm> pStpringArm;
};
}    // namespace LittleQuest

#include <System/Scene.h>

namespace LittleQuest {

    USING_PTR(Camera);

    // カメラ
    class Camera : public Object {
        public:
            BP_OBJECT_TYPE(Camera, Object);

            static CameraPtr Create(ObjectPtr obj);

            bool Init() override;

            //#ifdef USE_MOUSE_CAMERA
            void Update() override;
            //#endif

        private:
            // armの回転は上20度(X軸回転)の角度としておく
            float3 rot_{-20, 0, 0};

            ObjectWeakPtr target_obj_;
    };

}    // namespace LittleQuest

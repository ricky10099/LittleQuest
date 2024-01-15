#include <System/Scene.h>
#include <System/Component/ComponentModel.h>

#include <LittleQuest/Components/ComponentTexture.h>
#include <LittleQuest/Objects/Camera.h>

namespace LittleQuest {
//class Camera;

class GameTitleScene: public Scene::Base {
   public:
    BP_CLASS_TYPE(GameTitleScene, Scene::Base);

    std::string Name() {
        return "LittleQuest/GameTitleScene";
    }

    bool Init() override;

    void Update() override;

    void LateDraw() override;

    void Exit() override;

    void GUI() override;

   private:
    const float START_TIME       = 120.0f;
    float3      START_CAM_POS    = {-3, 3, 12};
    float3      START_CAM_TARGET = {0, 1.5f, 0};
    float3      END_CAM_POS      = {-1, 6, 20};
    float3      END_CAM_TARGET   = {0, 5, 0};

    float m_startTimer     = START_TIME;
    float m_alpha          = 0;
    float m_elapsed60      = 0;
    int   m_titleImage     = -1;
    int   m_fontHandle     = -1;
    int   m_stringWidth    = 0;
    int   m_stringHeight   = 0;
    int   m_loadCount      = 0;
    int   m_totalLoadCount = 0;
    bool  m_isStart        = false;
    bool  m_showString     = true;

    ComponentModelPtr                 m_pModel;
    ComponentCameraPtr                m_pCamera;
    std::weak_ptr<Camera>             m_pSceneCamera;
    std::weak_ptr<ComponentTexture2D> m_pTitle;
    std::weak_ptr<Texture>            m_ptTitle;
};

}    // namespace LittleQuest

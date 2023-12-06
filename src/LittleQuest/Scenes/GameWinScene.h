#include <System/Scene.h>

namespace LittleQuest {
class GameWinScene: public Scene::Base {
   public:
    BP_CLASS_TYPE(GameWinScene, Scene::Base);

    bool Init() override;

    void Update() override;

    void Draw() override;

    void Exit() override;

    void GUI() override;

   private:
    int image = -1;
};

}    // namespace LittleQuest

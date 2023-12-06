#include <System/Scene.h>

namespace LittleQuest {
class GameOverScene: public Scene::Base {
   public:
    BP_CLASS_TYPE(GameOverScene, Scene::Base);

    bool Init() override;

    void Update() override;

    void Draw() override;

    void Exit() override;

    void GUI() override;

   private:
    int image = -1;
};

}    // namespace LittleQuest

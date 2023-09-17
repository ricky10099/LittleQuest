#include <System/Scene.h>

namespace LittleQuest {
    class GameTitleScene : public Scene::Base {
        public:
            BP_CLASS_TYPE(GameTitleScene, Scene::Base);

            std::string Name() { return "LittleQuest/GameTitleScene"; }

            bool Init() override;

            void Update() override;

            void Draw() override;

            void Exit() override;

            void GUI() override;

        private:
            int image = -1;
    };

}    // namespace LittleQuest

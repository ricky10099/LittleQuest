#include <System/Scene.h>

namespace Sample
{
namespace GameSample
{
class GameScene : public Scene::Base
{
public:
    BP_CLASS_TYPE(GameScene, Scene::Base);

    std::string Name() { return "GameSample/GameScene"; }

    bool Init() override;

    void Update() override;

    void Draw() override;

    void Exit() override;

    void GUI() override;
};

}   // namespace GameSample
}   // namespace Sample

#include <System/Scene.h>

namespace Sample
{
namespace GameSample
{
class TitleScene : public Scene::Base
{
public:
    BP_CLASS_TYPE(TitleScene, Scene::Base);

    std::string Name() { return "GameSample/TitleScene"; }

    bool Init() override;

    void Update() override;

    void Draw() override;

    void Exit() override;

    void GUI() override;
};

}   // namespace GameSample
}   // namespace Sample

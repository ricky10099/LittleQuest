#include <System/Scene.h>

namespace Sample
{
namespace GameSample
{
class OverScene : public Scene::Base
{
public:
    BP_CLASS_TYPE(OverScene, Scene::Base);

    bool Init() override;

    void Update() override;

    void Draw() override;

    void Exit() override;

    void GUI() override;
};

}   // namespace GameSample
}   // namespace Sample

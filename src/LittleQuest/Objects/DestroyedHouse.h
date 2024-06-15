#pragma once

#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(DestroyedHouse);
class DestroyedHouse: public Object {
   public:
    BP_OBJECT_DECL(DestroyedHouse, "LittleQuest/DestroyedHouse");
    static DestroyedHousePtr Create(std::string name = "DestroyedHouse", const float3& pos = {0, 0, 0});

    bool Init() override;
    void Update() override;
    void LateDraw() override;
    void GUI() override;
    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;
   private:
    ObjectPtr m_pBox;
};
}    // namespace LittleQuest

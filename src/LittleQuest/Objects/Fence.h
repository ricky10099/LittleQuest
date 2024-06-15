#pragma once

#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(Fence);
class Fence: public Object {
   public:
    BP_OBJECT_DECL(Fence, "LittleQuest/Fence");
    static FencePtr Create(const float3& pos = {0, 0, 0});

    bool Init() override;
    void Update() override;
    void LateDraw() override;
    void GUI() override;
    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;
   private:
    ObjectPtr m_pBox;
};
}    // namespace LittleQuest

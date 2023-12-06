#pragma once

#include "Enemy.h"
#include <System/Scene.h>

namespace LittleQuest {
USING_PTR(Mutant);

class Mutant: public Enemy {
   public:
    BP_OBJECT_TYPE(Mutant, Enemy);
    static MutantPtr Create(const float3& pos, bool isPatrol = true);

    bool Init() override;
    void Update() override;
    void LateDraw() override;
    void GUI() override;
    void OnHit(const ComponentCollision::HitInfo& hitInfo) override;
    void GetHit(int damage) override;

   protected:
    void Patrol(float3& move) override;
    void BackToInitialPosition(float3& move) override;
    void Die() override;

   private:
};
}    // namespace LittleQuest

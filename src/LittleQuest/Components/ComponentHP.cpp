#include "ComponentHP.h"

namespace LittleQuest {
    void ComponentHP::Init() {
        Super::Init();
    }

    void ComponentHP::Update() {
        Super::Update();

        auto owner      = GetOwner();
        float3 ownerPos = owner->GetTranslate();
        VECTOR worldVec = cast(ownerPos);
        worldVec.y += 11;
        pos2D      = ConvWorldPosToScreenPos(worldVec);
    }

    void ComponentHP::Draw() {
        Super::Draw();
        DrawFillBox(pos2D.x - 101, pos2D.y - 1, pos2D.x + 101, pos2D.y + 11,
                    GetColor(255, 0, 0));
        DrawFillBox(pos2D.x - 100, pos2D.y,
                    pos2D.x - 100 + ((currHP / (float)maxHP) * 200),
                    pos2D.y + 10, GetColor(255, 255, 0));

        printfDx("\ncurrHP: %i", currHP);
        printfDx("\nmaxHP: %i", maxHP);
    }

    void ComponentHP::GUI() {
        Super::GUI();
    }

    void ComponentHP::SetHP(int HP) {
        this->maxHP  = HP;
        this->currHP = this->maxHP;
    }

    int ComponentHP::GetHP() {
        return this->currHP;
    }

    void ComponentHP::TakeDamage(int damage) {
        this->currHP -= damage;
        if (this->currHP < 0) {
            this->currHP = 0;
        }
    }

    BP_COMPONENT_IMPL(ComponentHP, u8"HPコンポーネント");
}    // namespace LittleQuest

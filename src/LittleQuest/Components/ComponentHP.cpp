#include "ComponentHP.h"

namespace LittleQuest {
    void ComponentHP::Init() {
        Super::Init();
    }

    void ComponentHP::Update() {
        Super::Update();

        auto owner      = GetOwner();
        VECTOR worldVec = cast(owner->GetTranslate());
        worldVec.y += 11;
        pos2D = ConvWorldPosToScreenPos(worldVec);
    }

    void ComponentHP::Draw() {
        Super::Draw();
        if (pos2D.z > 0 && pos2D.z < 1) {
            DrawFillBox((int)pos2D.x - 101, (int)pos2D.y - 1, (int)pos2D.x + 101, (int)pos2D.y + 11, GetColor(255, 0, 0));
            DrawFillBox((int)pos2D.x - 100, (int)pos2D.y, (int)(pos2D.x - 100 + ((currHP / (float)maxHP) * 200)),
                        (int)pos2D.y + 10, GetColor(255, 255, 0));
        }
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

    void ComponentHP::DrawHPBar() {
        if (pos2D.z > 0 && pos2D.z < 1) {
            DrawFillBox((int)pos2D.x - 101, (int)pos2D.y - 1, (int)pos2D.x + 101, (int)pos2D.y + 11, GetColor(255, 0, 0));
            DrawFillBox((int)pos2D.x - 100, (int)pos2D.y, (int)(pos2D.x - 100 + ((currHP / (float)maxHP) * 200)),
                        (int)pos2D.y + 10, GetColor(255, 255, 0));
        }
    }

    BP_COMPONENT_IMPL(ComponentHP, u8"HPコンポーネント");
}    // namespace LittleQuest

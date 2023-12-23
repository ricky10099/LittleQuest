#include "ComponentHP.h"

namespace LittleQuest {
void ComponentHP::Init() {
    Super::Init();
}

void ComponentHP::Update() {
    Super::Update();

    auto   owner    = GetOwner();
    VECTOR worldVec = cast(owner->GetTranslate());
    worldVec.y += 11;
    pos2D = ConvWorldPosToScreenPos(worldVec);
}

void ComponentHP::Draw() {
    Super::Draw();
}

void ComponentHP::GUI() {
    Super::GUI();
}

void ComponentHP::SetType(HP_TYPE type) {
    this->type = type;
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
    this->currHP = std::max(0, this->currHP);
}

void ComponentHP::DrawHPBar() {
    int screenWidth, screenHeight;
    int posX, posY, posX2, posY2;
    GetScreenState(&screenWidth, &screenHeight, nullptr);
    switch(type) {
    case HP_TYPE::MOB:
        if(pos2D.z > 0 && pos2D.z < 1) {
            DrawFillBox((int)pos2D.x - 101, (int)pos2D.y - 1, (int)pos2D.x + 101, (int)pos2D.y + 11, GetColor(255, 0, 0));
            DrawFillBox((int)pos2D.x - 100, (int)pos2D.y, (int)(pos2D.x - 100 + ((currHP / (float)maxHP) * 200)),
                        (int)pos2D.y + 10, GetColor(255, 255, 0));
        }
        break;
    case HP_TYPE::PLAYER:
        posX  = screenWidth * 0.05;
        posY  = screenHeight * 0.05;
        posX2 = screenWidth * 0.3;
        posY2 = screenHeight * 0.07;
        DrawFillBox(posX - 2, posY - 2, posX2 + 2, posY2 + 2, GetColor(0, 0, 0));
        DrawFillBox(posX, posY, (int)(posX + ((currHP / (float)maxHP) * (posX2 - posX))), posY2, GetColor(0, 255, 0));
        break;
    case HP_TYPE::BOSS:
        posX  = screenWidth * 0.15;
        posY  = screenHeight * 0.9;
        posX2 = screenWidth * 0.85;
        posY2 = screenHeight * 0.95;
        DrawFillBox(posX - 2, posY - 2, posX2 + 2, posY2 + 2, GetColor(0, 0, 0));
        DrawFillBox(posX, posY, (int)(posX + ((currHP / (float)maxHP) * (posX2 - posX))), posY2, GetColor(255, 0, 0));
        break;
    }
}

BP_COMPONENT_IMPL(ComponentHP, u8"HPコンポーネント");
}    // namespace LittleQuest

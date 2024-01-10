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
    m_pos2D = ConvWorldPosToScreenPos(worldVec);

    if(m_damageTimer > 0.0f) {
        m_damageTimer -= GetDeltaTime60();
    }

    if(m_backHP > m_currHP && m_damageTimer <= 0.0f) {
        m_backHP -= GetDeltaTime60();
        m_decreasing = true;
    }

    m_backHP = std::max(0.0f, m_backHP);
}

void ComponentHP::Draw() {
    Super::Draw();
}

void ComponentHP::GUI() {
    Super::GUI();
}

void ComponentHP::SetType(HP_TYPE type) {
    this->m_type = type;
}

void ComponentHP::SetHP(int HP) {
    this->m_maxHP  = HP;
    this->m_currHP = this->m_maxHP;
    this->m_backHP = this->m_currHP;
}

int ComponentHP::GetHP() {
    return this->m_currHP;
}

float ComponentHP::GetHPRate() {
    return ((float)this->m_currHP / this->m_maxHP) * 100.0f;
}

void ComponentHP::TakeDamage(int damage) {
    if(this->m_decreasing) {
        this->m_backHP = this->m_currHP;
    }
    this->m_currHP -= damage;
    this->m_currHP      = std::max(0, this->m_currHP);
    this->m_damageTimer = DAMAGE_TIME;
    this->m_decreasing  = false;
}

void ComponentHP::DrawHPBar() {
    int screenWidth, screenHeight;
    int posX, posY, posX2, posY2;
    GetScreenState(&screenWidth, &screenHeight, nullptr);

    switch(m_type) {
    case HP_TYPE::MOB:
        if(m_pos2D.z < 0 || m_pos2D.z > 1) {
            break;
        }
        DrawFillBox((int)m_pos2D.x - 101, (int)m_pos2D.y - 1, (int)m_pos2D.x + 101, (int)m_pos2D.y + 11, GetColor(255, 0, 0));
        DrawFillBox((int)m_pos2D.x - 100, (int)m_pos2D.y, (int)(m_pos2D.x - 100 + ((m_currHP / (float)m_maxHP) * 200)),
                    (int)m_pos2D.y + 10, GetColor(255, 255, 0));
        break;
    case HP_TYPE::PLAYER:
        posX  = (int)(screenWidth * 0.05f);
        posY  = (int)(screenHeight * 0.05f);
        posX2 = (int)(screenWidth * 0.3f);
        posY2 = (int)(screenHeight * 0.065f);
        DrawBoxAA(posX - 2.0f, posY - 2.0f, posX2 + 2.0f, posY2 + 2.0f, GetColor(255, 255, 255), FALSE, 2.0f);
        DrawFillBox(posX, posY, (int)(posX + ((m_backHP / (float)m_maxHP) * (posX2 - posX))), posY2, GetColor(200, 0, 0));
        DrawFillBox(posX, posY, (int)(posX + ((m_currHP / (float)m_maxHP) * (posX2 - posX))), posY2, GetColor(0, 255, 0));
        break;
    case HP_TYPE::BOSS:
        posX  = (int)(screenWidth * 0.15f);
        posY  = (int)(screenHeight * 0.9f);
        posX2 = (int)(screenWidth * 0.85f);
        posY2 = (int)(screenHeight * 0.92f);
        DrawBoxAA(posX - 2.0f, posY - 2.0f, posX2 + 2.0f, posY2 + 2.0f, GetColor(0, 0, 0), FALSE, 2.0f);
        DrawFillBox(posX, posY, (int)(posX + ((m_backHP / (float)m_maxHP) * (posX2 - posX))), posY2, GetColor(200, 0, 0));
        DrawFillBox(posX, posY, (int)(posX + ((m_currHP / (float)m_maxHP) * (posX2 - posX))), posY2, GetColor(255, 0, 0));
        break;
    }
}

BP_COMPONENT_IMPL(ComponentHP, u8"HPコンポーネント");
}    // namespace LittleQuest

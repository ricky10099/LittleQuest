#include "ComponentCombo.h"

namespace LittleQuest {
void ComponentCombo::Init() {
    Super::Init();
}

void ComponentCombo::Update() {
    Super::Update();

    if(m_currCombo < 10) {
        m_comboColor = COMBO_BUFF_0;
    } else if(m_currCombo < 20) {
        m_comboColor = COMBO_BUFF_1;
    } else if(m_currCombo < 30) {
        m_comboColor = COMBO_BUFF_2;
    } else {
        m_comboColor = COMBO_BUFF_3;
    }

    m_currCombo = std::min(m_currCombo, MAX_COMBO);
}

void ComponentCombo::Draw() {
    Super::Draw();
    int screenWidth, screenHeight;
    GetScreenState(&screenWidth, &screenHeight, nullptr);

    int posX  = screenWidth * 0.05;
    int posY  = screenHeight * 0.08;
    int posX2 = screenWidth * 0.2;
    int posY2 = screenHeight * 0.1;
    DrawBox(posX - 2, posY - 2, posX2 + 2, posY2 + 2, GetColor(0, 0, 0), FALSE);
    DrawFillBox(posX, posY, (int)(posX + ((m_currCombo / MAX_COMBO) * (posX2 - posX))), posY2, m_comboColor);
    DrawLine((int)(posX + (posX2 - posX) * 0.33f), posY, (int)(posX + (posX2 - posX) * 0.33f), posY2, GetColor(0, 0, 0), 2);
    DrawLine((int)(posX + (posX2 - posX) * 0.66f), posY, (int)(posX + (posX2 - posX) * 0.66f), posY2, GetColor(0, 0, 0), 2);
}

void ComponentCombo::GUI() {
    Super::GUI();
}

void ComponentCombo::AddCombo() {
    ++m_currCombo;
}

float ComponentCombo::ComboBuff() {
    if(m_currCombo >= 30) {
        return 2.0f;
    } else if(m_currCombo >= 20) {
        return 1.5f;
    } else if(m_currCombo >= 10) {
        return 1.25f;
    }

    return 1.0f;
}

BP_COMPONENT_IMPL(ComponentCombo, u8"Comboコンポーネント");
}    // namespace LittleQuest

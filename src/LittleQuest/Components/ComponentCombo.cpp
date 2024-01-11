#include "ComponentCombo.h"

namespace LittleQuest {
void ComponentCombo::Init() {
    Super::Init();
}

void ComponentCombo::Update() {
    Super::Update();

    if(m_comboPauseTimer > 0) {
        m_comboPauseTimer -= GetDeltaTime60();
    } else {
        m_comboTimer -= GetDeltaTime60();
    }

    if(m_comboTimer <= 0) {
        m_currCombo -= GetDeltaTime60();
    }

    m_currCombo = std::max(0.0f, m_currCombo);

    if(m_currCombo < 10) {
        m_comboColor = COMBO_BUFF_0;
    } else if(m_currCombo < 20) {
        m_comboColor = COMBO_BUFF_1;
    } else if(m_currCombo < 30) {
        m_comboColor = COMBO_BUFF_2;
    } else {
        m_comboColor = COMBO_BUFF_3;
    }
}

void ComponentCombo::DrawComboBar() {
    int screenWidth, screenHeight;
    GetScreenState(&screenWidth, &screenHeight, nullptr);

    int posX  = (int)(screenWidth * 0.05);
    int posY  = (int)(screenHeight * 0.08);
    int posX2 = (int)(screenWidth * 0.2);
    int posY2 = (int)(screenHeight * 0.1);
    DrawBoxAA(posX - 2.0f, posY - 2.0f, posX2 + 2.0f, posY2 + 2.0f, GetColor(255, 255, 255), FALSE, 2.0f);
    DrawFillBox(posX, posY, (int)(posX + ((m_currCombo / MAX_COMBO) * (posX2 - posX))), posY2, m_comboColor);
    DrawLine((int)(posX + (posX2 - posX) * 0.33f), posY, (int)(posX + (posX2 - posX) * 0.33f), posY2, GetColor(255, 255, 255),
             2);
    DrawLine((int)(posX + (posX2 - posX) * 0.66f), posY, (int)(posX + (posX2 - posX) * 0.66f), posY2, GetColor(255, 255, 255),
             2);
}

void ComponentCombo::GUI() {
    Super::GUI();
}

void ComponentCombo::AddCombo() {
    if(m_currCombo < MAX_COMBO) {
        ++m_currCombo;
    }
    m_comboTimer      = COMBO_TIMER;
    m_comboPauseTimer = COMBO_PUASE;
}

float ComponentCombo::ComboBuff() {
    if(m_currCombo >= 30) {
        return 3.0f;
    } else if(m_currCombo >= 20) {
        return 2.0f;
    } else if(m_currCombo >= 10) {
        return 1.5f;
    }

    return 1.0f;
}

BP_COMPONENT_IMPL(ComponentCombo, u8"Comboコンポーネント");
}    // namespace LittleQuest

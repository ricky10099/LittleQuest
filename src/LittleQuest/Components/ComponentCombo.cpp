#include "ComponentCombo.h"

namespace LittleQuest {
//------------------------------------------------------------
//  更新処理を行います。
//------------------------------------------------------------
void ComponentCombo::Update() {
    Super::Update();

    if(m_comboTimer > 0) {
        m_comboTimer -= GetDeltaTime60();
    } else {
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

//------------------------------------------------------------
//  コンボ数を増やします。
//------------------------------------------------------------
void ComponentCombo::AddCombo(int combo) {
    if(m_currCombo < MAX_COMBO) {
        m_currCombo += combo;
    }
    m_currCombo  = std::min(m_currCombo, MAX_COMBO);
    m_comboTimer = COMBO_TIMER;
}

//------------------------------------------------------------
//  コンボゲージを描画します。
//------------------------------------------------------------
void ComponentCombo::DrawComboBar() {
    int screenWidth, screenHeight;
    GetScreenState(&screenWidth, &screenHeight, nullptr);

    float posX  = screenWidth * 0.05f;
    float posY  = screenHeight * 0.08f;
    float posX2 = screenWidth * 0.2f;
    float posY2 = screenHeight * 0.1f;
    DrawBoxAA(posX - 2.0f, posY - 2.0f, posX2 + 2.0f, posY2 + 2.0f, GetColor(255, 255, 255), FALSE, 2.0f);
    DrawBoxAA(posX, posY, (posX + ((m_currCombo / MAX_COMBO) * (posX2 - posX))), posY2, m_comboColor, TRUE);
    DrawLineAA((posX + (posX2 - posX) * 0.33f), posY, (posX + (posX2 - posX) * 0.33f), posY2, GetColor(255, 255, 255), 2);
    DrawLineAA((posX + (posX2 - posX) * 0.66f), posY, (posX + (posX2 - posX) * 0.66f), posY2, GetColor(255, 255, 255), 2);
}

//------------------------------------------------------------
//  コンボによる攻撃力バフ。
//------------------------------------------------------------
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

BP_COMPONENT_IMPL(ComponentCombo, "Comboコンポーネント");
}    // namespace LittleQuest

#include "ComponentHP.h"

namespace LittleQuest {
//------------------------------------------------------------
//　更新処理を行います。
//------------------------------------------------------------
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

//------------------------------------------------------------
//　ゲージのタイプを設定します。
//------------------------------------------------------------
void ComponentHP::SetType(HP_TYPE type) {
    this->m_type = type;
}

//------------------------------------------------------------
//　最大のHP量を設定します。
//------------------------------------------------------------
void ComponentHP::SetHP(int HP) {
    m_maxHP  = HP;
    m_currHP = m_maxHP;
    m_backHP = (float)m_currHP;
}

//------------------------------------------------------------
//　現在のHP量を取得します。
//------------------------------------------------------------
int ComponentHP::GetHP() {
    return m_currHP;
}

//------------------------------------------------------------
//　最大のHP量を設定します。
//------------------------------------------------------------
float ComponentHP::GetHPRate() {
    return ((float)m_currHP / m_maxHP) * 100.0f;
}

//------------------------------------------------------------
//　ダメージの計算を行います。
//------------------------------------------------------------
void ComponentHP::TakeDamage(int damage) {
    if(m_decreasing) {
        m_backHP = (float)m_currHP;
    }
    m_currHP -= damage;
    m_currHP      = std::max(0, m_currHP);
    m_damageTimer = DAMAGE_TIME;
    m_decreasing  = false;
}

//------------------------------------------------------------
//　HPゲージを描画します。
//------------------------------------------------------------
void ComponentHP::DrawHPBar() {
    int   screenWidth, screenHeight;
    float posX1, posY1, posX2, posY2;
    GetScreenState(&screenWidth, &screenHeight, nullptr);

    switch(m_type) {
    case HP_TYPE::PLAYER:
        posX1 = screenWidth * 0.05f;
        posY1 = screenHeight * 0.05f;
        posX2 = screenWidth * 0.3f;
        posY2 = screenHeight * 0.065f;
        DrawBoxAA(posX1 - 2.0f, posY1 - 2.0f, posX2 + 2.0f, posY2 + 2.0f, GetColor(255, 255, 255), FALSE, 2.0f);
        DrawBoxAA(posX1, posY1, (posX1 + ((m_backHP / (float)m_maxHP) * (posX2 - posX1))), posY2, GetColor(100, 0, 0), TRUE);
        DrawBoxAA(posX1, posY1, (posX1 + ((m_currHP / (float)m_maxHP) * (posX2 - posX1))), posY2, GetColor(0, 255, 0), TRUE);
        break;
    case HP_TYPE::BOSS:
        posX1 = screenWidth * 0.15f;
        posY1 = screenHeight * 0.9f;
        posX2 = screenWidth * 0.85f;
        posY2 = screenHeight * 0.92f;
        DrawBoxAA(posX1 - 2.0f, posY1 - 2.0f, posX2 + 2.0f, posY2 + 2.0f, GetColor(0, 0, 0), FALSE, 2.0f);
        DrawBoxAA(posX1, posY1, (posX1 + ((m_backHP / (float)m_maxHP) * (posX2 - posX1))), posY2, GetColor(100, 0, 0), TRUE);
        DrawBoxAA(posX1, posY1, (posX1 + ((m_currHP / (float)m_maxHP) * (posX2 - posX1))), posY2, GetColor(255, 0, 0), TRUE);
        break;
    }
}
}    // namespace LittleQuest

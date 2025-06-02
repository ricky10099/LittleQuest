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
    _pos2D = ConvWorldPosToScreenPos(worldVec);

    if(_damageTimer > 0.0f) {
        _damageTimer -= GetDeltaTime60();
    }

    if(_backHP > _currHP && _damageTimer <= 0.0f) {
        _backHP -= GetDeltaTime60();
        _decreasing = true;
    }

    _backHP = std::max(0.0f, _backHP);
}

//------------------------------------------------------------
//　ゲージのタイプを設定します。
//------------------------------------------------------------
void ComponentHP::SetType(HP_TYPE type) {
    this->_type = type;
}

//------------------------------------------------------------
//　最大のHP量を設定します。
//------------------------------------------------------------
void ComponentHP::SetHP(int HP) {
    _maxHP  = HP;
    _currHP = _maxHP;
    _backHP = (float)_currHP;
}

//------------------------------------------------------------
//　現在のHP量を取得します。
//------------------------------------------------------------
int ComponentHP::GetHP() {
    return _currHP;
}

//------------------------------------------------------------
//　最大のHP量を設定します。
//------------------------------------------------------------
float ComponentHP::GetHPRate() {
    return ((float)_currHP / _maxHP) * 100.0f;
}

//------------------------------------------------------------
//　ダメージの計算を行います。
//------------------------------------------------------------
void ComponentHP::TakeDamage(int damage) {
    if(_decreasing) {
        _backHP = (float)_currHP;
    }
    _currHP -= damage;
    _currHP      = std::max(0, _currHP);
    _damageTimer = DAMAGE_TIME;
    _decreasing  = false;
}

//------------------------------------------------------------
//　HPゲージを描画します。
//------------------------------------------------------------
void ComponentHP::DrawHPBar() {
    int   screenWidth, screenHeight;
    float posX1, posY1, posX2, posY2;
    GetScreenState(&screenWidth, &screenHeight, nullptr);

    switch(_type) {
    case HP_TYPE::PLAYER:
        posX1 = screenWidth * 0.05f;
        posY1 = screenHeight * 0.05f;
        posX2 = screenWidth * 0.3f;
        posY2 = screenHeight * 0.065f;
        DrawBoxAA(posX1 - 2.0f, posY1 - 2.0f, posX2 + 2.0f, posY2 + 2.0f, GetColor(255, 255, 255), FALSE, 2.0f);
        DrawBoxAA(posX1, posY1, (posX1 + ((_backHP / (float)_maxHP) * (posX2 - posX1))), posY2, GetColor(100, 0, 0), TRUE);
        DrawBoxAA(posX1, posY1, (posX1 + ((_currHP / (float)_maxHP) * (posX2 - posX1))), posY2, GetColor(0, 255, 0), TRUE);
        break;
    case HP_TYPE::BOSS:
        posX1 = screenWidth * 0.15f;
        posY1 = screenHeight * 0.9f;
        posX2 = screenWidth * 0.85f;
        posY2 = screenHeight * 0.92f;
        DrawBoxAA(posX1 - 2.0f, posY1 - 2.0f, posX2 + 2.0f, posY2 + 2.0f, GetColor(0, 0, 0), FALSE, 2.0f);
        DrawBoxAA(posX1, posY1, (posX1 + ((_backHP / (float)_maxHP) * (posX2 - posX1))), posY2, GetColor(100, 0, 0), TRUE);
        DrawBoxAA(posX1, posY1, (posX1 + ((_currHP / (float)_maxHP) * (posX2 - posX1))), posY2, GetColor(255, 0, 0), TRUE);
        break;
    case HP_TYPE::MOB:
        DrawFillBox((int)_pos2D.x - 101, (int)_pos2D.y - 1, (int)_pos2D.x + 101, (int)_pos2D.y + 11, GetColor(255, 0, 0));
        DrawFillBox((int)_pos2D.x - 100, (int)_pos2D.y, (int)(_pos2D.x - 100 + ((_currHP / (float)_maxHP) * 200)),
                    (int)_pos2D.y + 10, GetColor(255, 255, 0));
        break;
    }
}
}    // namespace LittleQuest

#include "ComponentHP.h"

namespace LittleQuest {
//------------------------------------------------------------
//　更新処理を行います。
//------------------------------------------------------------
void ComponentHP::Update() {
    Super::Update();

    auto   owner    = GetOwner();
    VECTOR worldVec = cast(owner->GetTranslate());
    worldVec.y += _offsetY;
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
        // 画面の左上に設定する
        posX1 = screenWidth * 0.05f;
        posY1 = screenHeight * 0.05f;
        posX2 = screenWidth * 0.3f;
        posY2 = screenHeight * 0.065f;
        // 体力バーの枠線
        DrawBoxAA(posX1 - _outlineThickness, posY1 - _outlineThickness, posX2 + _outlineThickness, posY2 + _outlineThickness,
                  GetColor(255, 255, 255), FALSE, _outlineThickness);
        // 体力の赤アニメション
        DrawBoxAA(posX1, posY1, (posX1 + ((_backHP / (float)_maxHP) * (posX2 - posX1))), posY2, GetColor(100, 0, 0), TRUE);
        // 体力
        DrawBoxAA(posX1, posY1, (posX1 + ((_currHP / (float)_maxHP) * (posX2 - posX1))), posY2, GetColor(0, 255, 0), TRUE);
        break;
    case HP_TYPE::BOSS:
        // 画面の中央下に設定する
        posX1 = screenWidth * 0.15f;
        posY1 = screenHeight * 0.9f;
        posX2 = screenWidth * 0.85f;
        posY2 = screenHeight * 0.92f;
        DrawBoxAA(posX1 - _outlineThickness, posY1 - _outlineThickness, posX2 + _outlineThickness, posY2 + _outlineThickness,
                  GetColor(0, 0, 0), FALSE, _outlineThickness);
        DrawBoxAA(posX1, posY1, (posX1 + ((_backHP / (float)_maxHP) * (posX2 - posX1))), posY2, GetColor(100, 0, 0), TRUE);
        DrawBoxAA(posX1, posY1, (posX1 + ((_currHP / (float)_maxHP) * (posX2 - posX1))), posY2, GetColor(255, 0, 0), TRUE);
        break;
    case HP_TYPE::MOB:
        DrawFillBox((int)_pos2D.x - _maxHP / 2, (int)_pos2D.y - 1, (int)_pos2D.x + _maxHP / 2, (int)_pos2D.y + 6,
                    GetColor(0, 0, 0));
        DrawFillBox((int)_pos2D.x - _maxHP / 2, (int)_pos2D.y,
                    (int)(_pos2D.x - _maxHP / 2 + ((_currHP / (float)_maxHP) * _maxHP)), (int)_pos2D.y + 5,
                    GetColor(255, 0, 0));
        break;
    default:
        break;
    }
}
void ComponentHP::SetHPBarOffsetY(float offsetY) {
    _offsetY = offsetY;
}
}    // namespace LittleQuest

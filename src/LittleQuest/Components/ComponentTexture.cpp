#include "ComponentTexture.h"

namespace LittleQuest {
//------------------------------------------------------------
//  初期化処理を行います。
//------------------------------------------------------------
void ComponentTexture2D::Init() {
    Super::Init();

    // 頂点データの準備
    m_vertex[0].pos = {0.0f, 0.0f, 0.0f};
    m_vertex[1].pos = {128.0f, 0.0f, 0.0f};
    m_vertex[2].pos = {0.0f, 128.0f, 0.0f};
    m_vertex[3].pos = {128.0f, 128.0f, 0.0f};
    m_vertex[0].rhw = 1.0f;    // rhw = 1.0f 初期化は2D描画に必須
    m_vertex[1].rhw = 1.0f;
    m_vertex[2].rhw = 1.0f;
    m_vertex[3].rhw = 1.0f;
    m_vertex[0].dif = GetColorU8(255, 255, 255, 255);
    m_vertex[1].dif = GetColorU8(255, 255, 255, 255);
    m_vertex[2].dif = GetColorU8(255, 255, 255, 255);
    m_vertex[3].dif = GetColorU8(255, 255, 255, 255);
    m_vertex[0].u   = 0.0f;
    m_vertex[0].v   = 0.0f;
    m_vertex[1].u   = 1.0f;
    m_vertex[1].v   = 0.0f;
    m_vertex[2].u   = 0.0f;
    m_vertex[2].v   = 1.0f;
    m_vertex[3].u   = 1.0f;
    m_vertex[3].v   = 1.0f;
}

//------------------------------------------------------------
//  テクスチャを描画します。
//------------------------------------------------------------
void ComponentTexture2D::DrawTexture() {
    SetUseTextureToShader(0, *m_pTexture);
    SetUsePixelShader(*m_pShaderPS);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawPrimitive2DToShader(m_vertex, 4, DX_PRIMTYPE_TRIANGLESTRIP);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, NULL);
    SetUseTextureToShader(0, -1);
}

//------------------------------------------------------------
//  テクスチャの描画座標を設定します。
//------------------------------------------------------------
void ComponentTexture2D::SetPosition(float x1, float y1, float x2, float y2) {
    m_vertex[0].pos = {x1, y1, 0.0f};
    m_vertex[1].pos = {x2, y1, 0.0f};
    m_vertex[2].pos = {x1, y2, 0.0f};
    m_vertex[3].pos = {x2, y2, 0.0f};
}

BP_COMPONENT_IMPL(ComponentTexture2D, u8"Textureasdコンポーネント");
}    // namespace LittleQuest

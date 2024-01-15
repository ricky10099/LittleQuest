//----------------------------------------------------------------------------
//!	@file	ps_texture.fx
//!	@brief	テクスチャありピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

// キューブマップIBLテクスチャ
TextureCube ibl_diffuse_texture : register(t11);
TextureCube ibl_specular_texture : register(t12);

static const float PI = 3.141592;

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT	output;

	// テクスチャカラーの読み込み
	float4	color = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);
    float3 iblDiffuse = ibl_diffuse_texture.SampleLevel(DiffuseSampler, 1, 0.0).rgb * (1.0 / PI);
	// 出力カラー = テクスチャカラー * ディフューズカラー
	output.color0_ = color * input.diffuse_;
    output.color0_.rgb = (pow(color.rgb, 2.2) * input.diffuse_.rgb);
	
    output.color0_.rgb += (pow(color.rgb, 2.2) * input.diffuse_.rgb) * iblDiffuse;
	// 出力パラメータを返す
	return output;
}

//----------------------------------------------------------------------------
//!	@file	ps_3d_texture.fx
//!	@brief	テクスチャありピクセルシェーダー(3D)
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

// キューブマップIBLテクスチャ
TextureCube ibl_diffuse_texture : register(t11);
TextureCube ibl_specular_texture : register(t12);

static const float PI = 3.141592;

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_3D input)
{
	PS_OUTPUT	output;

//    input.uv0_.x *= 80;
	
	// テクスチャカラーの読み込み
    float4 color = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);
    float3 iblDiffuse = ibl_diffuse_texture.SampleLevel(DiffuseSampler, 1, 0.0).rgb * (1.0 / PI);
	// 出力カラー = テクスチャカラー * ディフューズカラー
    output.color0_ = color * input.diffuse_;
    output.color0_.rgb = 5 * (pow(color.rgb, 2.2) * input.diffuse_.rgb) * 0.5;
	
    output.color0_.rgb += (pow(color.rgb, 2.2) * input.diffuse_.rgb) * iblDiffuse;
	
//    output.color0_ = float4(input.uv0_, 0, 1);
//    output.color0_ = float4(1, 1, 0, 1);

	
	// 出力パラメータを返す
	return output;
}

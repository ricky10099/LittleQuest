//----------------------------------------------------------------------------
//!	@file	ps_3d_texture.fx
//!	@brief	テクスチャありピクセルシェーダー(3D)
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_3D input)
{
	PS_OUTPUT	output;

//    input.uv0_.x *= 80;
	
	
	// テクスチャカラーの読み込み
	float4	color = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);

	// 出力カラー = テクスチャカラー * ディフューズカラー
	output.color0_ = color * input.diffuse_;

//    output.color0_ = float4(input.uv0_, 0, 1);
//    output.color0_ = float4(1, 1, 0, 1);

	
	// 出力パラメータを返す
	return output;
}

//----------------------------------------------------------------------------
//!	@file	ps_tonemapping.fx
//!	@brief	HDRトーンマッピング
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

float3 ACESFilm(float3 x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

static const float A = 0.15;
static const float B = 0.50;
static const float C = 0.10;
static const float D = 0.20;
static const float E = 0.02;
static const float F = 0.30;
static const float W = 11.2;

float3 Uncharted2Tonemap(float3 x)
{
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT	output;

	// テクスチャカラーの読み込み
	float4	color = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);

	// トーンマッピング適用
#if 0
	color.rgb = ACESFilm(color.rgb);
#else
    static const float EXPOSURE_BIAS = 2.0f;
    float3 curr = Uncharted2Tonemap(EXPOSURE_BIAS * color.rgb);

    float3 whiteScale = 1.0f / Uncharted2Tonemap(W);
    color.rgb = curr * whiteScale;
#endif
	
	// sRGBへ変換
    color.rgb = pow(abs(color.rgb), 1.0 / 2.2);

	output.color0_ = color;

	// 出力パラメータを返す
	return output;
}

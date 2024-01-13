//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"


// カメラ情報
cbuffer CameraInfo : register(b10) // Constant Buffer = 10番
{
    matrix mat_view_; //!< ビュー行列
    matrix mat_proj_; //!< 投影行列
    float3 eye_position_; //!< カメラの位置
};

// キューブマップIBLテクスチャ
TextureCube ibl_diffuse_texture : register(t11);
TextureCube ibl_specular_texture : register(t12);




static const float PI = 3.141592;

// GGX (Trowbridge Reitzモデル)	NDF(Normal Distribution Function 法線分布関数)
float D_GGX(float NdotH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * denom * denom);
}

float G_Smith(float NdotL, float NdotV, float roughness)
{
    float k = roughness * 0.5;
	// UE4版
    // float k = (roughness + 1.0) * (roughness + 1.0) * (1.0/8.0);

    float smithL = NdotL / (NdotL * (1.0 - k) + k);
    float smithV = NdotV / (NdotV * (1.0 - k) + k);
    return smithL * smithV;
}


// [フレネル] Schlickの近似式
float3 F_Schlick(float NdotV, float3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - NdotV, 5.0);
}



//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_3D input)
{
    PS_OUTPUT output;

    float2 uv = input.uv0_;
    float3 N = normalize(input.normal_); // 法線

	//------------------------------------------------------------
	// 法線マップ
	//------------------------------------------------------------
    N = Normalmap(N, input.worldPosition_, uv);

	//------------------------------------------------------------
	// テクスチャカラーを読み込み
	//------------------------------------------------------------
    float4 textureColor = DiffuseTexture.Sample(DiffuseSampler, uv);
    float3 albedo = textureColor.rgb; // 一時的にカラーテクスチャをAlbedoとして扱う

	// アルベドをリニア空間に変換
	// ※元のカラー画像はリニア空間ではなくsRGBの画像
	// 法線マップなどの数値データーをカラー画像に使用しているものは
	// リニア空間になっているため変換不要
    albedo = pow(albedo, 2.2);
	
	
	
	
    // アルファテスト
    if (textureColor.a < 0.5)
        discard;

	
	// 光源計算
    float3 L = normalize(float3(1, 1, -1));
	
	// 拡散反射光 Diffuse
	// Lambertモデル
    static const float Kd = 1.0 / 3.141592;
    float diffuse = saturate(dot(N, L)) * Kd; // 正規化Lambert
	
	// 鏡面反射光 Specular
    float3 V = normalize(eye_position_ * float3(1, 1, -1) - input.worldPosition_);
    float3 H = normalize(N + V);
    float NdotH = saturate(dot(N, H));
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
	
    float roughness = 0.8;  // ラフネス(表面の粗さ)
    float metallic  = 0.0;  // メタリック (金属度合い)

    float diffuseFactor = 1.0 - metallic;
    
    float D = D_GGX(NdotH, roughness);
    float G = G_Smith(NdotL, NdotV, roughness);
	// フレネル反射
    float3 specularColor = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);
    float3 F = F_Schlick(NdotV, specularColor);
	
#if 0
	// blinn-Phongモデル
    float power    = 300.0;
    float specular = pow(NdotH, power);
#elif 1
	// Cook-Torranceモデル
    float3 brdf =     (D * G * F)
	          / //------------------------
				  (4.0 * NdotL * NdotV +0.000001);
    float3 specular = brdf * NdotL;
#else
	// 高速化版「Optimizing PBR」SIGGRAPH 2015
    float roughness4 = roughness * roughness * roughness * roughness;
    float denominator = (NdotH * NdotH * (roughness4 - 1.0) + 1.0) * saturate(dot(L, H));

    float brdf = roughness4
				/ //---------------------------------------------------------
				  (4.0 * PI * denominator * denominator * (roughness + 0.5) +0.000001);
    //brdf = 1;
    float3 specular = specularColor * brdf * NdotL;
#endif
	// 結果
    float4 vertexColor = input.diffuse_;
	
    float3 lightColor = float3(1, 1, 1) * 0.2;
	
	
    output.color0_.rgb = lightColor * (albedo * vertexColor.rgb) * diffuse * diffuseFactor + lightColor * specular;
    output.color0_.a = 1.0;

    float3 R = reflect(-V, N);
    float  mipLevel = roughness * 7.0;
    float3 iblDiffuse = ibl_diffuse_texture.SampleLevel(DiffuseSampler, N, 0.0).rgb * (1.0/PI);
    float3 iblSpecular = ibl_specular_texture.SampleLevel(DiffuseSampler, R, mipLevel).rgb;

    iblDiffuse  *= 1;
    iblSpecular *= 0.1;
    
    float3 environmentBRDF = specularColor + pow(1.0 - max(roughness, NdotV), 3.0);
    output.color0_.rgb += (albedo * vertexColor.rgb) /** iblDiffuse */* diffuseFactor +
                                                   /*iblSpecular **/ environmentBRDF;
    
    

//    output.color0_.rgb = fresnel;


     //output.color0_.rgb = N * 0.5 + 0.5;	// [-1～+1] → [0～+1]に変換


	
	// リニア→sRGBへ変換 (表示用の色空間)
//    output.color0_.rgb = pow(saturate(output.color0_.rgb), 1.0 / 2.2);
	
	

	
	// 出力パラメータを返す
    return output;
}

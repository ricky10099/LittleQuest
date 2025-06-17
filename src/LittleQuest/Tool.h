#pragma once
#include <cmath>

#define STR(var) #var

namespace {
//------------------------------------------------------------
//! @brief 二つの座標から距離を取得します。
//!
//! @param start 最初の座標
//! @param goal 目標の座標
//! @param bool y軸も計算するのか
//! @return 二つの座標の距離
//------------------------------------------------------------
float GetDistance(float3 start, float3 goal, bool includeY = false) {
    return std::sqrt(std::pow(goal.x - start.x, 2) + std::pow(goal.z - start.z, 2) +
                     (includeY * std::pow(goal.y - start.y, 2)));
}

//------------------------------------------------------------
//! @brief ベクトルの長さを取得します。
//!
//! @param vector ベクトル
//! @return ベクトルの長さ
//------------------------------------------------------------
float GetDistance(float3 vector) {
    return std::sqrt(std::pow(vector.x, 2) + std::pow(vector.y, 2) + std::pow(vector.z, 2));
}

//------------------------------------------------------------
//! @brief ベクトルはゼロなのかを取得します。
//!
//! @param vector ベクトル
//! @retval true ベクトルはゼロ
//! @retval false ベクトルはゼロではない
//------------------------------------------------------------
bool IsFloat3Zero(float3 vector) {
    return vector.x == 0 && vector.y == 0 && vector.z == 0;
}

float DotProdcut(float2 u, float2 v) {
    //float rtn = 0;

    //for (int i = 0; i < 2; ++i){
    //    rtn += u[i] * v[i];
    //}

    return (u.x * v.x) + (u.y * v.y);
}

float DotProdcut(float3 u, float3 v) {
    float rtn = 0;

    for(int i = 0; i < 3; ++i) {
        rtn += u[i] * v[i];
    }

    return rtn;
}

float DotProdcut(float4 u, float4 v) {
    float rtn = 0;

    for(int i = 0; i < 4; ++i) {
        rtn += u[i] * v[i];
    }

    return rtn;
}

float3 Slerp(float3 startPos, float3 endPos, float t) {
    // Compute angle between start and end
    float dotProduct = clamp(dot(normalize(startPos), normalize(endPos)), -1.0f, 1.0f);
    float omega      = acos(dotProduct);    // angle in radians

    // If angle is very small, fallback to Lerp
    if(abs(omega) < 1e-5f) {
        return lerp(startPos, endPos, t);
    }

    // Slerp formula
    float sinOmega = sin(omega);
    float scale0   = sin((1.0f - t) * omega) / sinOmega;
    float scale1   = sin(t * omega) / sinOmega;

    return scale0 * startPos + scale1 * endPos;
}

float3 SlerpWithCenter(float3 center, float3 startPos, float3 endPos, float t) {
    // Convert positions to relative vectors from the center
    float3 from = startPos - center;
    float3 to   = endPos - center;

    float3 slerpedDir = Slerp(from, to, t);

    // Reconstruct world-space position by adding the center back
    return center + slerpedDir;
}
//------------------------------------------------------------
//! @brief アニメーション情報の構造体。
//------------------------------------------------------------
struct AnimInfo {
    //! アニメーションの再生速度
    float animSpeed        = 1;
    //! アニメーション最初の再生速度
    float animStartSpeed   = 1;
    //! アニメーション何フレームから再生
    float animStartTime    = 0;
    //! 攻撃アニメーションであれば、何フレームから衝突判断始める
    float triggerStartTime = 0;
    //! 攻撃アニメーションであれば、何フレームまで衝突判断終わる
    float triggerEndTime   = 0;
    //! アニメーション何フレームから次のアニメーション再生できる
    float animCutInTime    = 0;
};
}    // namespace

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

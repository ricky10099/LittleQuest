#pragma once
#include <cmath>

#define STR(var) #var

namespace {
float GetDistance(float3 start, float3 goal, bool includeY = false) {
    return std::sqrt(std::pow(goal.x - start.x, 2) + std::pow(goal.z - start.z, 2) +
                     (includeY * std::pow(goal.y - start.y, 2)));
}

float GetDistance(float3 distance) {
    return std::sqrt(std::pow(distance.x, 2) + std::pow(distance.y, 2) + std::pow(distance.z, 2));
}

bool IsFloat3Zero(float3 vector) {
    return vector.x == 0 && vector.y == 0 && vector.z == 0;
}

struct AnimInfo {
    float animSpeed        = 1;
    float animStartSpeed   = 1;
    float animStartTime    = 0;
    float triggerStartTime = 0;
    float triggerEndTime   = 0;
    float animCutInTime    = 0;
};

//float3 Lerp(float3 posA, float3 posB, float3 currPos) {
//    float x, y, z;
//    return posA + (currPos / (posB - posA)) * (posB - posA);
//}
}    // namespace

#pragma once

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
}    // namespace

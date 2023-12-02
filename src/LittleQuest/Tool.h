#pragma once

namespace LittleQuest {
    float GetDistance(float3 start, float3 goal, bool includeY = false) {
        return std::sqrt(std::pow(goal.x - start.x, 2) + std::pow(goal.z - start.z, 2)
                         + (includeY * std::pow(goal.y - start.y, 2)));
    }
}    // namespace LittleQuest

#pragma once
#include <glm/glm.hpp>
#include <limits>
#include <array>

struct AABB {
    glm::vec3 min{ std::numeric_limits<float>::max() };
    glm::vec3 max{ std::numeric_limits<float>::lowest() };

    bool isValid() const {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }

    void expand(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    void expand(const AABB& other) {
        if (!other.isValid()) return;
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
    }

    glm::vec3 getCenter() const { return (min + max) * 0.5f; }
    glm::vec3 getExtents() const { return (max - min) * 0.5f; }
    glm::vec3 getSize() const { return max - min; }

    // Returns the world-space AABB that encloses this box after transformation by m.
    AABB transformed(const glm::mat4& m) const {
        if (!isValid()) return AABB();

        std::array<glm::vec3, 8> corners = {
            glm::vec3(min.x, min.y, min.z),
            glm::vec3(max.x, min.y, min.z),
            glm::vec3(min.x, max.y, min.z),
            glm::vec3(max.x, max.y, min.z),
            glm::vec3(min.x, min.y, max.z),
            glm::vec3(max.x, min.y, max.z),
            glm::vec3(min.x, max.y, max.z),
            glm::vec3(max.x, max.y, max.z),
        };

        AABB result;
        for (const auto& corner : corners) {
            result.expand(glm::vec3(m * glm::vec4(corner, 1.0f)));
        }
        return result;
    }
};

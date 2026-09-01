#pragma once
#include <glm/glm.hpp>
#include <array>

struct Plane {
    glm::vec3 normal;
    float distance;
    float signedDistance(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

class Frustum {
public:
    static Frustum fromViewProjection(const glm::mat4& viewProj);
    bool intersects(const glm::vec3& aabbMin, const glm::vec3& aabbMax) const;

private:
    std::array<Plane, 6> planes; // left, right, bottom, top, near, far
};
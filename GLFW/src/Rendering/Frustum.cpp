#include "Frustum.h"

namespace {
    void normalizePlane(Plane& plane) {
        float length = glm::length(plane.normal);
        if (length > 0.0f) {
            plane.normal /= length;
            plane.distance /= length;
        }
    }
}

Frustum Frustum::fromViewProjection(const glm::mat4& m) {
    Frustum frustum;

    // glm::mat4 is column-major: m[col][row]. Extract the four rows of the
    // combined view-projection matrix to build the six clip planes
    // (Gribb/Hartmann method, adapted for column-vector convention).
    glm::vec4 row0(m[0][0], m[1][0], m[2][0], m[3][0]);
    glm::vec4 row1(m[0][1], m[1][1], m[2][1], m[3][1]);
    glm::vec4 row2(m[0][2], m[1][2], m[2][2], m[3][2]);
    glm::vec4 row3(m[0][3], m[1][3], m[2][3], m[3][3]);

    glm::vec4 combined[6] = {
        row3 + row0, // left
        row3 - row0, // right
        row3 + row1, // bottom
        row3 - row1, // top
        row3 + row2, // near
        row3 - row2, // far
    };

    for (int i = 0; i < 6; ++i) {
        frustum.planes[i].normal = glm::vec3(combined[i]);
        frustum.planes[i].distance = combined[i].w;
        normalizePlane(frustum.planes[i]);
    }

    return frustum;
}

bool Frustum::intersects(const glm::vec3& aabbMin, const glm::vec3& aabbMax) const {
    for (const Plane& plane : planes) {
        glm::vec3 positiveVertex(
            plane.normal.x >= 0.0f ? aabbMax.x : aabbMin.x,
            plane.normal.y >= 0.0f ? aabbMax.y : aabbMin.y,
            plane.normal.z >= 0.0f ? aabbMax.z : aabbMin.z
        );
        if (plane.signedDistance(positiveVertex) < 0.0f) {
            return false; // AABB is fully outside this plane
        }
    }
    return true;
}

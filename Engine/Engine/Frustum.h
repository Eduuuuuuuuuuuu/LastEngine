#pragma once
#include <glm/glm.hpp>
#include "AABB.h"

class Frustum {
public:
    enum class FrustumPlane {
        NEAR = 0,
        FAR = 1,
        LEFT = 2,
        RIGHT = 3,
        TOP = 4,
        BOTTOM = 5
    };

    struct Plane {
        glm::vec3 normal = glm::vec3(0.0f);
        float distance = 0.0f;

        float GetSignedDistanceTo(const glm::vec3& point) const {
            return glm::dot(normal, point) + distance;
        }
    };

    Plane planes[6];

    void Update(const glm::mat4& projection, const glm::mat4& view);
    bool ContainsAABB(const AABB& aabb) const;
};
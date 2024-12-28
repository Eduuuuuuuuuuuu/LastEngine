#include "Frustum.h"

void Frustum::Update(const glm::mat4& projection, const glm::mat4& view) {
    glm::mat4 viewProj = projection * view;

    // Left plane
    planes[static_cast<int>(FrustumPlane::LEFT)].normal.x = viewProj[0][3] + viewProj[0][0];
    planes[static_cast<int>(FrustumPlane::LEFT)].normal.y = viewProj[1][3] + viewProj[1][0];
    planes[static_cast<int>(FrustumPlane::LEFT)].normal.z = viewProj[2][3] + viewProj[2][0];
    planes[static_cast<int>(FrustumPlane::LEFT)].distance = viewProj[3][3] + viewProj[3][0];

    // Right plane
    planes[static_cast<int>(FrustumPlane::RIGHT)].normal.x = viewProj[0][3] - viewProj[0][0];
    planes[static_cast<int>(FrustumPlane::RIGHT)].normal.y = viewProj[1][3] - viewProj[1][0];
    planes[static_cast<int>(FrustumPlane::RIGHT)].normal.z = viewProj[2][3] - viewProj[2][0];
    planes[static_cast<int>(FrustumPlane::RIGHT)].distance = viewProj[3][3] - viewProj[3][0];

    // Bottom plane
    planes[static_cast<int>(FrustumPlane::BOTTOM)].normal.x = viewProj[0][3] + viewProj[0][1];
    planes[static_cast<int>(FrustumPlane::BOTTOM)].normal.y = viewProj[1][3] + viewProj[1][1];
    planes[static_cast<int>(FrustumPlane::BOTTOM)].normal.z = viewProj[2][3] + viewProj[2][1];
    planes[static_cast<int>(FrustumPlane::BOTTOM)].distance = viewProj[3][3] + viewProj[3][1];

    // Top plane
    planes[static_cast<int>(FrustumPlane::TOP)].normal.x = viewProj[0][3] - viewProj[0][1];
    planes[static_cast<int>(FrustumPlane::TOP)].normal.y = viewProj[1][3] - viewProj[1][1];
    planes[static_cast<int>(FrustumPlane::TOP)].normal.z = viewProj[2][3] - viewProj[2][1];
    planes[static_cast<int>(FrustumPlane::TOP)].distance = viewProj[3][3] - viewProj[3][1];

    // Near plane
    planes[static_cast<int>(FrustumPlane::NEAR)].normal.x = viewProj[0][3] + viewProj[0][2];
    planes[static_cast<int>(FrustumPlane::NEAR)].normal.y = viewProj[1][3] + viewProj[1][2];
    planes[static_cast<int>(FrustumPlane::NEAR)].normal.z = viewProj[2][3] + viewProj[2][2];
    planes[static_cast<int>(FrustumPlane::NEAR)].distance = viewProj[3][3] + viewProj[3][2];

    // Far plane
    planes[static_cast<int>(FrustumPlane::FAR)].normal.x = viewProj[0][3] - viewProj[0][2];
    planes[static_cast<int>(FrustumPlane::FAR)].normal.y = viewProj[1][3] - viewProj[1][2];
    planes[static_cast<int>(FrustumPlane::FAR)].normal.z = viewProj[2][3] - viewProj[2][2];
    planes[static_cast<int>(FrustumPlane::FAR)].distance = viewProj[3][3] - viewProj[3][2];

    // Normalize all planes
    for (auto& plane : planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }
}

bool Frustum::ContainsAABB(const AABB& aabb) const {
    // Check if any point of the AABB is on the positive side of any plane
    for (const auto& plane : planes) {
        glm::vec3 positiveVertex = aabb.min;
        if (plane.normal.x >= 0) positiveVertex.x = aabb.max.x;
        if (plane.normal.y >= 0) positiveVertex.y = aabb.max.y;
        if (plane.normal.z >= 0) positiveVertex.z = aabb.max.z;

        if (plane.GetSignedDistanceTo(positiveVertex) < 0) {
            return false; // Outside the frustum
        }
    }
    return true; // Inside the frustum
}
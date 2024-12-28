#include "AABB.h"
#include "Frustum.h"
#include <float.h>

AABB::AABB() : min(FLT_MAX), max(-FLT_MAX), center(0), size{ 0,0,0 } {}

AABB::AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {
    center = (min + max) * 0.5f;
    size[0] = max.x - min.x;
    size[1] = max.y - min.y;
    size[2] = max.z - min.z;
}

void AABB::Encapsulate(const glm::vec3& point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
    center = (min + max) * 0.5f;
    size[0] = max.x - min.x;
    size[1] = max.y - min.y;
    size[2] = max.z - min.z;
}

void AABB::Encapsulate(const AABB& aabb) {
    Encapsulate(aabb.min);
    Encapsulate(aabb.max);
}

bool AABB::Contains(const glm::vec3& point) const {
    return point.x >= min.x && point.x <= max.x &&
        point.y >= min.y && point.y <= max.y &&
        point.z >= min.z && point.z <= max.z;
}

bool AABB::Intersects(const AABB& other) const {
    return min.x <= other.max.x && max.x >= other.min.x &&
        min.y <= other.max.y && max.y >= other.min.y &&
        min.z <= other.max.z && max.z >= other.min.z;
}

void AABB::SetFromCenterAndSize(const glm::vec3& center, const glm::vec3& size) {
    this->center = center;
    this->size[0] = size.x;
    this->size[1] = size.y;
    this->size[2] = size.z;
    min = center - size * 0.5f;
    max = center + size * 0.5f;
}

void AABB::SetNegativeInfinity() {
    min = glm::vec3(FLT_MAX);
    max = glm::vec3(-FLT_MAX);
    center = glm::vec3(0);
    size[0] = size[1] = size[2] = 0;
}

float AABB::GetVolume() const {
    glm::vec3 dimensions = max - min;
    return dimensions.x * dimensions.y * dimensions.z;
}

bool AABB::Intersects(const Frustum& frustum) const {
    // Para cada plano del frustum, comprobamos si el AABB está completamente fuera
    for (const auto& plane : frustum.planes) {
        glm::vec3 positiveVertex = min;

        if (plane.normal.x >= 0) positiveVertex.x = max.x;
        if (plane.normal.y >= 0) positiveVertex.y = max.y;
        if (plane.normal.z >= 0) positiveVertex.z = max.z;

        if (plane.GetSignedDistanceTo(positiveVertex) < 0) {
            return false; // Fuera del frustum
        }
    }
    return true; // Dentro del frustum
}
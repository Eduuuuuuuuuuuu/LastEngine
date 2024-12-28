#pragma once
#include <glm/glm.hpp>

class Frustum;

class AABB {
public:
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
    float size[3];  // Size in each dimension (x,y,z)

    AABB();
    AABB(const glm::vec3& min, const glm::vec3& max);

    void Encapsulate(const glm::vec3& point);
    void Encapsulate(const AABB& aabb);
    bool Contains(const glm::vec3& point) const;
    bool Intersects(const AABB& other) const;
    bool Intersects(const Frustum& frustum) const;

    void SetFromCenterAndSize(const glm::vec3& center, const glm::vec3& size);
    void SetNegativeInfinity();

    glm::vec3 GetCenter() const { return center; }
    glm::vec3 GetSize() const { return max - min; }
    float GetVolume() const;
};
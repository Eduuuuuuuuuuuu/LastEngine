#pragma once
#include <glm/glm.hpp>
#include "vector"

class AABB {
public:
    glm::vec3 min_point;
    glm::vec3 max_point;

    AABB() : min_point(0.0f), max_point(0.0f) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : min_point(min), max_point(max) {}

    // Update AABB based on vertex positions
    void UpdateFromVertices(const std::vector<glm::vec3>& vertices) {
        if (vertices.empty()) return;

        min_point = max_point = vertices[0];
        for (const auto& vertex : vertices) {
            min_point = glm::min(min_point, vertex);
            max_point = glm::max(max_point, vertex);
        }
    }

    // Get center point of AABB
    glm::vec3 GetCenter() const {
        return (min_point + max_point) * 0.5f;
    }

    // Get size of AABB
    glm::vec3 GetSize() const {
        return max_point - min_point;
    }

    // Transform AABB by a matrix
    AABB Transform(const glm::mat4& transform) const {
        glm::vec3 corners[8] = {
            glm::vec3(min_point.x, min_point.y, min_point.z),
            glm::vec3(max_point.x, min_point.y, min_point.z),
            glm::vec3(min_point.x, max_point.y, min_point.z),
            glm::vec3(max_point.x, max_point.y, min_point.z),
            glm::vec3(min_point.x, min_point.y, max_point.z),
            glm::vec3(max_point.x, min_point.y, max_point.z),
            glm::vec3(min_point.x, max_point.y, max_point.z),
            glm::vec3(max_point.x, max_point.y, max_point.z)
        };

        AABB transformed;
        transformed.min_point = glm::vec3(std::numeric_limits<float>::max());
        transformed.max_point = glm::vec3(std::numeric_limits<float>::lowest());

        for (const auto& corner : corners) {
            glm::vec4 transformed_corner = transform * glm::vec4(corner, 1.0f);
            transformed.min_point = glm::min(transformed.min_point, glm::vec3(transformed_corner));
            transformed.max_point = glm::max(transformed.max_point, glm::vec3(transformed_corner));
        }

        return transformed;
    }
};

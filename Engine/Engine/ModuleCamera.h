#pragma once
#include "Module.h"
#include "glm/glm.hpp"
#include "GL/glew.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include "ModuleInput.h"
#include <array>
#include "AABB.h"
#include "Mesh.h"

class Plane {
public:
    glm::vec3 normal = glm::vec3(0.0f);
    float distance = 0.0f;

    void SetFromPoints(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
        glm::vec3 v1 = p2 - p1;
        glm::vec3 v2 = p3 - p1;
        normal = glm::normalize(glm::cross(v1, v2));
        distance = -glm::dot(normal, p1);
    }

    float GetSignedDistanceTo(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

class ModuleCamera : public Module
{
public:
    ModuleCamera(App* app);
    ~ModuleCamera();
    bool Start();
    bool Update(float dt);
    void HandleInput();
    void FrameSelected();
    bool CleanUp();
    void LookAt(const glm::vec3& spot);
    const glm::mat4& GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    // Frustum methods
    void UpdateFrustumPlanes();
    bool IsPointVisible(const glm::vec3& point) const;
    bool IsSphereVisible(const glm::vec3& center, float radius) const;
    bool IsAABBVisible(const glm::vec3& minPoint, const glm::vec3& maxPoint) const;
    void DrawFrustum() const;
    void DrawAABBs(const std::vector<std::pair<AABB, bool>>& aabbs) const;

    //click objects
    Ray ScreenPointToRay(int mouseX, int mouseY) const {
        // Convert screen coordinates to normalized device coordinates (-1 to 1)
        float normalizedX = (2.0f * mouseX) / screenWidth - 1.0f;
        float normalizedY = 1.0f - (2.0f * mouseY) / screenHeight;

        // Get inverse view-projection matrix
        glm::mat4 invVP = glm::inverse(GetProjectionMatrix() * viewMatrix);

        // Transform to world space
        glm::vec4 rayStart = invVP * glm::vec4(normalizedX, normalizedY, -1.0f, 1.0f);
        glm::vec4 rayEnd = invVP * glm::vec4(normalizedX, normalizedY, 1.0f, 1.0f);

        rayStart /= rayStart.w;
        rayEnd /= rayEnd.w;

        glm::vec3 rayDirection = glm::normalize(glm::vec3(rayEnd - rayStart));
        return Ray(glm::vec3(rayStart), rayDirection);
    }

    void CheckObjectSelection(int mouseX, int mouseY);

public:
    float fov = 60.0f;
    float nearPlane = 0.125f;
    float farPlane = 512.0f;
    int screenWidth, screenHeight;

private:
    void HandleMovement(glm::vec3& newPos, float speed, float fastSpeed);
    void HandleZoom(float zoomSpeed);
    void HandleRotation();
    void RotateCamera(int dx, int dy);
    void CalculateViewMatrix();
    glm::vec3 RotateVector(glm::vec3 const& vector, float angle, glm::vec3 const& axis);
    void SetCursor(CursorType cursorType);

    //frustum
    std::array<glm::vec3, 8> CalculateFrustumCorners() const;
    void DrawFrustumLines(const std::array<glm::vec3, 8>& corners) const;
    void DrawAABB(const AABB& aabb, bool isVisible) const;

private:
    glm::vec3 X, Y, Z;
    glm::vec3 pos, ref;
    glm::mat4 viewMatrix;
    bool isZooming = false;
    bool isOrbiting = false;
    bool isFreeLook = false;
    bool isDragging = false;
    bool isDefaultCursor = true;

    // Frustum planes
    enum FrustumPlane {
        PLANE_NEAR = 0,
        PLANE_FAR,
        PLANE_LEFT,
        PLANE_RIGHT,
        PLANE_TOP,
        PLANE_BOTTOM,
        PLANE_COUNT
    };
    std::array<Plane, PLANE_COUNT> frustumPlanes;
};
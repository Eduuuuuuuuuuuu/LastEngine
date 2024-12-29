#include "ModuleCamera.h"
#include "App.h"

ModuleCamera::ModuleCamera(App* app) : Module(app)
{
	screenWidth = app->window->width;
	screenHeight = app->window->height;

	CalculateViewMatrix();

	X = glm::vec3(1.0f, 0.0f, 0.0f);
	Y = glm::vec3(0.0f, 1.0f, 0.0f);
	Z = glm::vec3(0.0f, 0.0f, 1.0f);

	pos = glm::vec3(0.0f, 5.0f, 5.0f);
	ref = glm::vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera::~ModuleCamera()
{}

bool ModuleCamera::Start()
{
	LOG(LogType::LOG_INFO, "Setting up the camera");

	LookAt(ref);

	return true;
}

bool ModuleCamera::CleanUp()
{
	LOG(LogType::LOG_INFO, "Cleaning camera");

	return true;
}

bool ModuleCamera::Update(float dt)
{
	bool isMouseInside = app->editor->sceneWindow->IsMouseInside();
	if ((!isMouseInside || (!isZooming && !isFreeLook && !isOrbiting && !isDragging)) && !isDefaultCursor)
	{
		SetCursor(CursorType::DEFAULT);
	}

	if (isMouseInside)
		HandleInput();

	CalculateViewMatrix();

	return true;
}

void ModuleCamera::HandleInput()
{
	glm::vec3 newPos(0, 0, 0);

	float dt = app->GetDT();
	float speed = 10.0f * dt;
	float zoomSpeed = 30.0f * dt;
	float fastSpeed = 20.0f * dt;

	HandleMovement(newPos, speed, fastSpeed);

	HandleZoom(zoomSpeed);

	HandleRotation();

	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		FrameSelected();
	}

	pos += newPos;
	ref += newPos;
}

void ModuleCamera::HandleMovement(glm::vec3& newPos, float speed, float fastSpeed)
{
	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
	{
		if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed = fastSpeed;

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

		SetCursor(CursorType::FREELOOK);
	}
	else if (isFreeLook)
		isFreeLook = false;

	if (app->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		int dx = app->input->GetMouseXMotion();
		int dy = app->input->GetMouseYMotion();

		float panSpeed = 0.01f;
		newPos -= X * static_cast<float>(dx) * panSpeed;
		newPos += Y * static_cast<float>(dy) * panSpeed;

		SetCursor(CursorType::DRAG);
	}
	else if (isDragging)
		isDragging = false;
}

void ModuleCamera::HandleZoom(float zoomSpeed)
{
	int mouseZ = app->input->GetMouseZ();

	if (mouseZ != 0)
		pos -= Z * zoomSpeed * (mouseZ > 0 ? 1.0f : -1.0f);
}

void ModuleCamera::HandleRotation()
{
	int dx = -app->input->GetMouseXMotion();
	int dy = -app->input->GetMouseYMotion();

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT &&
		app->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
	{
		RotateCamera(dx, dy);
	}

	if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		pos -= ref;
		RotateCamera(dx, dy);
		pos = ref + Z * glm::length(pos);
		LookAt(ref);

		SetCursor(CursorType::ORBIT);
	}
	else if (isOrbiting)
		isOrbiting = false;

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		float sensitivity = 0.01f;
		float zoomDelta = dy * sensitivity;
		glm::vec3 direction = glm::normalize(pos - ref);
		pos += direction * zoomDelta;

		SetCursor(CursorType::ZOOM);
	}
	else if (isZooming)
		isZooming = false;
}

void ModuleCamera::RotateCamera(int dx, int dy)
{
	float sensitivity = 0.005f;

	if (dx != 0)
	{
		float DeltaX = (float)dx * sensitivity;

		X = RotateVector(X, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		Y = RotateVector(Y, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		Z = RotateVector(Z, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * sensitivity;

		Y = RotateVector(Y, DeltaY, X);
		Z = RotateVector(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = glm::vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = glm::cross(Z, X);
		}
	}
}

void ModuleCamera::FrameSelected()
{
	if (app->editor->selectedGameObject)
	{
		pos = glm::vec3(
			app->editor->selectedGameObject->transform->position.x,
			app->editor->selectedGameObject->transform->position.y + 5.0f,
			app->editor->selectedGameObject->transform->position.z + 5.0f
		);
		ref = app->editor->selectedGameObject->transform->position;
		LookAt(ref);
	}
	else
	{
		pos = glm::vec3(0.0f, 5.0f, 5.0f);
		ref = glm::vec3(0.0f, 0.0f, 0.0f);
		LookAt(ref);
	}
}

void ModuleCamera::LookAt(const glm::vec3& spot)
{
	ref = spot;

	Z = glm::normalize(pos - ref);
	X = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Z));
	Y = glm::cross(Z, X);

	CalculateViewMatrix();
}

const glm::mat4& ModuleCamera::GetViewMatrix() const
{
	return viewMatrix;
}

void ModuleCamera::CalculateViewMatrix()
{
	viewMatrix = glm::mat4(
		X.x, Y.x, Z.x, 0.0f,
		X.y, Y.y, Z.y, 0.0f,
		X.z, Y.z, Z.z, 0.0f,
		-glm::dot(X, pos), -glm::dot(Y, pos), -glm::dot(Z, pos), 1.0f
	);
}

glm::mat4 ModuleCamera::GetProjectionMatrix() const
{
	float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::vec3 ModuleCamera::RotateVector(glm::vec3 const& vector, float angle, glm::vec3 const& axis)
{
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

	glm::vec4 vector4 = glm::vec4(vector, 1.0f);

	glm::vec4 rotatedVector = rotationMatrix * vector4;

	return glm::vec3(rotatedVector);
}

void ModuleCamera::SetCursor(CursorType cursorType)
{
	if (app->input->GetCursor() != cursorType)
	{
		app->input->ChangeCursor(cursorType);

		isDefaultCursor = (cursorType == CursorType::DEFAULT);
		isFreeLook = (cursorType == CursorType::FREELOOK);
		isZooming = (cursorType == CursorType::ZOOM);
		isOrbiting = (cursorType == CursorType::ORBIT);
		isDragging = (cursorType == CursorType::DRAG);
	}
}

void ModuleCamera::UpdateFrustumPlanes() {
	glm::mat4 viewProj = GetProjectionMatrix() * GetViewMatrix();

	// Extract planes from view-projection matrix
	// Left plane
	frustumPlanes[PLANE_LEFT].normal.x = viewProj[3][0] + viewProj[0][0];
	frustumPlanes[PLANE_LEFT].normal.y = viewProj[3][1] + viewProj[0][1];
	frustumPlanes[PLANE_LEFT].normal.z = viewProj[3][2] + viewProj[0][2];
	frustumPlanes[PLANE_LEFT].distance = viewProj[3][3] + viewProj[0][3];

	// Right plane
	frustumPlanes[PLANE_RIGHT].normal.x = viewProj[3][0] - viewProj[0][0];
	frustumPlanes[PLANE_RIGHT].normal.y = viewProj[3][1] - viewProj[0][1];
	frustumPlanes[PLANE_RIGHT].normal.z = viewProj[3][2] - viewProj[0][2];
	frustumPlanes[PLANE_RIGHT].distance = viewProj[3][3] - viewProj[0][3];

	// Top plane
	frustumPlanes[PLANE_TOP].normal.x = viewProj[3][0] - viewProj[1][0];
	frustumPlanes[PLANE_TOP].normal.y = viewProj[3][1] - viewProj[1][1];
	frustumPlanes[PLANE_TOP].normal.z = viewProj[3][2] - viewProj[1][2];
	frustumPlanes[PLANE_TOP].distance = viewProj[3][3] - viewProj[1][3];

	// Bottom plane
	frustumPlanes[PLANE_BOTTOM].normal.x = viewProj[3][0] + viewProj[1][0];
	frustumPlanes[PLANE_BOTTOM].normal.y = viewProj[3][1] + viewProj[1][1];
	frustumPlanes[PLANE_BOTTOM].normal.z = viewProj[3][2] + viewProj[1][2];
	frustumPlanes[PLANE_BOTTOM].distance = viewProj[3][3] + viewProj[1][3];

	// Near plane
	frustumPlanes[PLANE_NEAR].normal.x = viewProj[3][0] + viewProj[2][0];
	frustumPlanes[PLANE_NEAR].normal.y = viewProj[3][1] + viewProj[2][1];
	frustumPlanes[PLANE_NEAR].normal.z = viewProj[3][2] + viewProj[2][2];
	frustumPlanes[PLANE_NEAR].distance = viewProj[3][3] + viewProj[2][3];

	// Far plane
	frustumPlanes[PLANE_FAR].normal.x = viewProj[3][0] - viewProj[2][0];
	frustumPlanes[PLANE_FAR].normal.y = viewProj[3][1] - viewProj[2][1];
	frustumPlanes[PLANE_FAR].normal.z = viewProj[3][2] - viewProj[2][2];
	frustumPlanes[PLANE_FAR].distance = viewProj[3][3] - viewProj[2][3];

	// Normalize all planes
	for (auto& plane : frustumPlanes) {
		float length = glm::length(plane.normal);
		plane.normal /= length;
		plane.distance /= length;
	}
}

bool ModuleCamera::IsPointVisible(const glm::vec3& point) const {
	for (const auto& plane : frustumPlanes) {
		if (plane.GetSignedDistanceTo(point) < 0) {
			return false;
		}
	}
	return true;
}

bool ModuleCamera::IsSphereVisible(const glm::vec3& center, float radius) const {
	for (const auto& plane : frustumPlanes) {
		if (plane.GetSignedDistanceTo(center) < -radius) {
			return false;
		}
	}
	return true;
}

bool ModuleCamera::IsAABBVisible(const glm::vec3& minPoint, const glm::vec3& maxPoint) const {
	for (const auto& plane : frustumPlanes) {
		// Get the positive vertex
		glm::vec3 p(minPoint);
		if (plane.normal.x >= 0) p.x = maxPoint.x;
		if (plane.normal.y >= 0) p.y = maxPoint.y;
		if (plane.normal.z >= 0) p.z = maxPoint.z;

		// If the positive vertex is outside, the AABB is completely outside
		if (plane.GetSignedDistanceTo(p) < 0) {
			return false;
		}
	}
	return true;
}

std::array<glm::vec3, 8> ModuleCamera::CalculateFrustumCorners() const {
	float aspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	float tanHalfFov = tan(glm::radians(fov * 0.5f));

	float nearHeight = 2.0f * nearPlane * tanHalfFov;
	float nearWidth = nearHeight * aspect;
	float farHeight = 2.0f * farPlane * tanHalfFov;
	float farWidth = farHeight * aspect;

	std::array<glm::vec3, 8> corners;

	// Near plane corners (in view space)
	corners[0] = glm::vec3(-nearWidth * 0.5f, nearHeight * 0.5f, -nearPlane);
	corners[1] = glm::vec3(nearWidth * 0.5f, nearHeight * 0.5f, -nearPlane);
	corners[2] = glm::vec3(nearWidth * 0.5f, -nearHeight * 0.5f, -nearPlane);
	corners[3] = glm::vec3(-nearWidth * 0.5f, -nearHeight * 0.5f, -nearPlane);

	// Far plane corners (in view space)
	corners[4] = glm::vec3(-farWidth * 0.5f, farHeight * 0.5f, -farPlane);
	corners[5] = glm::vec3(farWidth * 0.5f, farHeight * 0.5f, -farPlane);
	corners[6] = glm::vec3(farWidth * 0.5f, -farHeight * 0.5f, -farPlane);
	corners[7] = glm::vec3(-farWidth * 0.5f, -farHeight * 0.5f, -farPlane);

	// Transform corners to world space
	glm::mat4 viewToWorld = glm::inverse(GetViewMatrix());
	for (auto& corner : corners) {
		glm::vec4 worldCorner = viewToWorld * glm::vec4(corner, 1.0f);
		corner = glm::vec3(worldCorner);
	}

	return corners;
}

void ModuleCamera::DrawFrustumLines(const std::array<glm::vec3, 8>& corners) const {
	glLineWidth(1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 0.75f);

	glBegin(GL_LINES);
	// Near plane
	glVertex3f(corners[0].x, corners[0].y, corners[0].z);
	glVertex3f(corners[1].x, corners[1].y, corners[1].z);

	glVertex3f(corners[1].x, corners[1].y, corners[1].z);
	glVertex3f(corners[2].x, corners[2].y, corners[2].z);

	glVertex3f(corners[2].x, corners[2].y, corners[2].z);
	glVertex3f(corners[3].x, corners[3].y, corners[3].z);

	glVertex3f(corners[3].x, corners[3].y, corners[3].z);
	glVertex3f(corners[0].x, corners[0].y, corners[0].z);

	// Far plane
	glVertex3f(corners[4].x, corners[4].y, corners[4].z);
	glVertex3f(corners[5].x, corners[5].y, corners[5].z);

	glVertex3f(corners[5].x, corners[5].y, corners[5].z);
	glVertex3f(corners[6].x, corners[6].y, corners[6].z);

	glVertex3f(corners[6].x, corners[6].y, corners[6].z);
	glVertex3f(corners[7].x, corners[7].y, corners[7].z);

	glVertex3f(corners[7].x, corners[7].y, corners[7].z);
	glVertex3f(corners[4].x, corners[4].y, corners[4].z);

	// Connecting lines
	glVertex3f(corners[0].x, corners[0].y, corners[0].z);
	glVertex3f(corners[4].x, corners[4].y, corners[4].z);

	glVertex3f(corners[1].x, corners[1].y, corners[1].z);
	glVertex3f(corners[5].x, corners[5].y, corners[5].z);

	glVertex3f(corners[2].x, corners[2].y, corners[2].z);
	glVertex3f(corners[6].x, corners[6].y, corners[6].z);

	glVertex3f(corners[3].x, corners[3].y, corners[3].z);
	glVertex3f(corners[7].x, corners[7].y, corners[7].z);
	glEnd();

	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void ModuleCamera::DrawFrustum() const {
	auto corners = CalculateFrustumCorners();
	DrawFrustumLines(corners);
}

void ModuleCamera::DrawAABB(const AABB& aabb, bool isVisible) const {
	glLineWidth(1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (isVisible) {
		glColor4f(0.0f, 1.0f, 0.0f, 0.75f); // Verde para visible
	}
	else {
		glColor4f(1.0f, 0.0f, 0.0f, 0.75f); // Rojo para no visible
	}

	glBegin(GL_LINES);
	// Bottom face
	glVertex3f(aabb.min_point.x, aabb.min_point.y, aabb.min_point.z);
	glVertex3f(aabb.max_point.x, aabb.min_point.y, aabb.min_point.z);

	glVertex3f(aabb.max_point.x, aabb.min_point.y, aabb.min_point.z);
	glVertex3f(aabb.max_point.x, aabb.min_point.y, aabb.max_point.z);

	glVertex3f(aabb.max_point.x, aabb.min_point.y, aabb.max_point.z);
	glVertex3f(aabb.min_point.x, aabb.min_point.y, aabb.max_point.z);

	glVertex3f(aabb.min_point.x, aabb.min_point.y, aabb.max_point.z);
	glVertex3f(aabb.min_point.x, aabb.min_point.y, aabb.min_point.z);

	// Top face
	glVertex3f(aabb.min_point.x, aabb.max_point.y, aabb.min_point.z);
	glVertex3f(aabb.max_point.x, aabb.max_point.y, aabb.min_point.z);

	glVertex3f(aabb.max_point.x, aabb.max_point.y, aabb.min_point.z);
	glVertex3f(aabb.max_point.x, aabb.max_point.y, aabb.max_point.z);

	glVertex3f(aabb.max_point.x, aabb.max_point.y, aabb.max_point.z);
	glVertex3f(aabb.min_point.x, aabb.max_point.y, aabb.max_point.z);

	glVertex3f(aabb.min_point.x, aabb.max_point.y, aabb.max_point.z);
	glVertex3f(aabb.min_point.x, aabb.max_point.y, aabb.min_point.z);

	// Vertical edges
	glVertex3f(aabb.min_point.x, aabb.min_point.y, aabb.min_point.z);
	glVertex3f(aabb.min_point.x, aabb.max_point.y, aabb.min_point.z);

	glVertex3f(aabb.max_point.x, aabb.min_point.y, aabb.min_point.z);
	glVertex3f(aabb.max_point.x, aabb.max_point.y, aabb.min_point.z);

	glVertex3f(aabb.max_point.x, aabb.min_point.y, aabb.max_point.z);
	glVertex3f(aabb.max_point.x, aabb.max_point.y, aabb.max_point.z);

	glVertex3f(aabb.min_point.x, aabb.min_point.y, aabb.max_point.z);
	glVertex3f(aabb.min_point.x, aabb.max_point.y, aabb.max_point.z);
	glEnd();

	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void ModuleCamera::DrawAABBs(const std::vector<std::pair<AABB, bool>>& aabbs) const {
	for (const auto& [aabb, isVisible] : aabbs) {
		DrawAABB(aabb, isVisible);
	}
}
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include "AABB.h"

typedef unsigned int uint;

class Mesh
{
public:
	void InitMesh();
	void DrawMesh(GLuint textureID, bool drawTextures, bool wireframe, bool shadedWireframe);
	void DrawNormals(bool vertexNormals, bool faceNormals, float vertexNormalLength, float faceNormalLength, glm::vec3 vertexNormalColor, glm::vec3 faceNormalColor);
	void CleanUpMesh();

	void CalculateAABB();
	const AABB& GetAABB() const { return boundingBox; }
public:
	uint indicesId = 0;
	uint indicesCount = 0;
	uint* indices = nullptr;
	uint verticesId = 0;
	uint verticesCount = 0;
	float* vertices = nullptr;
	uint normalsId = 0;
	uint normalsCount = 0;
	float* normals = nullptr;
	uint texCoordsId = 0;
	uint texCoordsCount = 0;
	float* texCoords = nullptr;

	// Material properties
	glm::vec4 diffuseColor = glm::vec4(1.0f);
	glm::vec4 specularColor = glm::vec4(1.0f);
	glm::vec4 ambientColor = glm::vec4(1.0f);
	std::string diffuseTexturePath;

	AABB boundingBox;
};
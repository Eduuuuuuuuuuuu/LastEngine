#pragma once

#include "Component.h"
#include "Mesh.h"

class Mesh;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* gameObject);
	virtual ~ComponentMesh();

	void Update() override;
	void OnEditor() override;

	AABB GetWorldAABB() const;
	bool IsVisible() const { return isVisible; }

public:
	Mesh* mesh;
	bool isVisible = true;

private:
	bool showVertexNormals = false;
	bool showFaceNormals = false;
};
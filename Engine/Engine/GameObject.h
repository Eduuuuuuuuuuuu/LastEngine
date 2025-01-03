#pragma once

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

#include <string>
#include <vector>

class GameObject
{
public:
	GameObject(const char* name, GameObject* parent);
	virtual ~GameObject();

	void Update();

	void Enable();
	void Disable();

	Component* AddComponent(Component* component);
	Component* GetComponent(ComponentType type);

	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);
	void SetParent(GameObject* newParent);
	void DeleteGameObject();

	const std::vector<GameObject*>& GetChildren() const { return children; }
	GameObject* GetParent() const { return parent; }

public:
	GameObject* parent;
	std::string name;

	ComponentTransform* transform;
	ComponentMesh* mesh;
	ComponentMaterial* material;

	std::vector<Component*> components;
	std::vector<GameObject*> children;

	bool isActive = true;
	bool isEditing = false;

	bool toDelete = false;
};
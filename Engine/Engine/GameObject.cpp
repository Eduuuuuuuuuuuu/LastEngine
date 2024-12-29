#include "GameObject.h"

GameObject::GameObject(const char* name, GameObject* parent) : parent(parent), name(name)
{
	transform = new ComponentTransform(this);
	mesh = new ComponentMesh(this);
	material = new ComponentMaterial(this);

	AddComponent(transform);
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	if (isActive)
	{
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			(*it)->Update();
		}
		for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
		{
			(*it)->Update();
		}
	}
}

void GameObject::Enable()
{
}
void GameObject::Disable()
{
}

Component* GameObject::AddComponent(Component* component)
{
	components.push_back(component);

	return component;
}

Component* GameObject::GetComponent(ComponentType type)
{
	for (auto it = components.begin(); it != components.end(); ++it) {
		if ((*it)->type == type) {
			return (*it);
		}
	}

	return nullptr;
}

void GameObject::AddChild(GameObject* child)
{
    if (child == nullptr || child == this) return;
    
    GameObject* current = this;
    while (current != nullptr) {
        if (current == child) return;
        current = current->parent;
    }

    // Desvincula del padre anterior
    if (child->parent != nullptr) {
        auto& parentChildren = child->parent->children;
        auto it = std::find(parentChildren.begin(), parentChildren.end(), child);
        if (it != parentChildren.end()) {
            parentChildren.erase(it);
        }
    }

    children.push_back(child);
    child->parent = this;

    if (child->transform != nullptr) {
        child->transform->UpdateTransform();
    }
}

void GameObject::RemoveChild(GameObject* child)
{
    if (child == nullptr) return;

    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->parent = nullptr;
        children.erase(it);
        
        if (child->transform != nullptr) {
            child->transform->UpdateTransform();
        }
    }
}

void GameObject::SetParent(GameObject* newParent)
{
    if (newParent == nullptr) {
        if (parent != nullptr) {
            parent->RemoveChild(this);
        }
    }
    else {
        newParent->AddChild(this);
    }
}

void GameObject::DeleteGameObject()
{
    while (!children.empty()) {
        GameObject* child = children.front();
        child->DeleteGameObject();
    }
    
    if (parent != nullptr) {
        parent->RemoveChild(this);
    }
    
    for (Component* component : components) {
        delete component;
    }
    components.clear();
}
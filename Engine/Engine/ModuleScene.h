#pragma once

#include "Module.h"
#include "GameObject.h"

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(App* app);
	virtual ~ModuleScene();

	bool Awake();
	bool Update(float dt);
	bool CleanUp();

	GameObject* CreateGameObject(const char* name, GameObject* parent);

    const std::vector<GameObject*>& GetRootObjects() const {
        std::vector<GameObject*> rootObjects;
        if (root != nullptr) {
            for (auto* child : root->GetChildren()) {
                rootObjects.push_back(child);
            }
        }
        return rootObjects;
    }


public:
	GameObject* root = nullptr;
};
#pragma once

#include "Module.h"
#include "Resource.h"

#include <string>

class ModuleResources : public Module
{
public:
	ModuleResources(App* app);
	virtual ~ModuleResources();

	bool Awake();
	bool CleanUp();

	Resource* CreateResource(const std::string& fileDir, ResourceType type);

	ResourceType GetResourceTypeFromExtension(const std::string& extension);

	std::string CreateLibraryFileDir(std::string name, ResourceType type);
};
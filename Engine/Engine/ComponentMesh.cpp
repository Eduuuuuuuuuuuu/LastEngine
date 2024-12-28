#include "ComponentMesh.h"
#include "App.h"

#include <glm/gtc/type_ptr.hpp>

ComponentMesh::ComponentMesh(GameObject* gameObject) : Component(gameObject, ComponentType::MESH), mesh(nullptr)
{
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::Update()
{
	if (!isVisible || !mesh) return; // No renderizar si no es visibleç

	ComponentTransform* transform = gameObject->transform;

	if (transform != nullptr)
	{
		glPushMatrix();
		glMultMatrixf(glm::value_ptr(transform->globalTransform));
	}

	ComponentMaterial* material = gameObject->material;

	mesh->DrawMesh(
		material->textureId,
		app->editor->preferencesWindow->drawTextures,
		app->editor->preferencesWindow->wireframe,
		app->editor->preferencesWindow->shadedWireframe
	);

	if (showVertexNormals || showFaceNormals)
	{
		mesh->DrawNormals(
			showVertexNormals,
			showFaceNormals,
			app->editor->preferencesWindow->vertexNormalLength,
			app->editor->preferencesWindow->faceNormalLength,
			app->editor->preferencesWindow->vertexNormalColor,
			app->editor->preferencesWindow->faceNormalColor
		);
	}

	if (material && material->materialTexture)
	{
		mesh->DrawMesh(material->textureId, true, false, false);
	}
	else
	{
		mesh->DrawMesh(0, false, false, false);
	}

	if (transform != nullptr) glPopMatrix();
}

void ComponentMesh::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Vertices: %d", mesh->verticesCount);
		ImGui::Text("Indices: %d", mesh->indicesCount);
		ImGui::Text("Normals: %d", mesh->normalsCount);
		ImGui::Text("Texture Coords: %d", mesh->texCoordsCount);

		ImGui::Spacing();

		ImGui::Checkbox("Vertex Normals", &showVertexNormals);
		ImGui::Checkbox("Face Normals", &showFaceNormals);
	}
}

AABB ComponentMesh::GetWorldAABB() const
{
	if (!mesh) return AABB();

	AABB localAABB = mesh->GetAABB();
	ComponentTransform* transform = gameObject->transform;
	if (transform)
	{
		return localAABB.Transform(transform->globalTransform);
	}
	return localAABB;
}
#pragma once

#include "EntityManager.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"

#include "MeshComponent.h"

class Model
{
public:
	Model();

	std::shared_ptr<GameObject> createGameObject(EntityManager* manager, const std::string& name);
	
private:
	friend class ModelFactory;

	struct Node
	{
		Node() : 
			localTransformation(1.0f), mesh(nullptr), material(nullptr)
		{}

		std::string name;

		std::vector<Node> children;

		mat4 localTransformation;
		Mesh* mesh;
		MeshMaterial* material;
	};

	Node m_rootNode;
	std::vector<Node*> m_nodes;

	std::vector<Mesh> m_meshes;
	std::vector<MeshMaterial> m_materials;
};
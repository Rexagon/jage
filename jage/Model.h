#pragma once

#include "MeshObject.h"

class Model
{
public:
	std::shared_ptr<GameObject> getRootObject();
	
private:
	friend class ModelFactory;

	std::shared_ptr<GameObject> m_rootObject;

	std::vector<Mesh> m_meshes;
	std::vector<Material> m_materials;
};
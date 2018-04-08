#pragma once

#include "MeshMaterial.h"
#include "Mesh.h"

class MeshComponent
{
public:
	MeshComponent(Mesh* mesh, std::shared_ptr<Material> material);

	void setMesh(Mesh* mesh);
	Mesh* getMesh() const;

	void setMaterial(std::shared_ptr<Material> material);
	Material* getMaterial();

private:
	Mesh* m_mesh;
	std::shared_ptr<Material> m_material;
};
#pragma once

#include "Material.h"
#include "Mesh.h"

class MeshComponent
{
public:
	MeshComponent(Mesh* mesh, const Material& material);

	void setMesh(Mesh* mesh);
	Mesh* getMesh() const;

	void setMaterial(const Material& material);
	Material& getMaterial();

private:
	Mesh* m_mesh;
	Material m_material;
};
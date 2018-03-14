#pragma once

#include "GameObject.h"
#include "Material.h"
#include "Shader.h"
#include "Mesh.h"

class MeshComponent
{
public:
	MeshComponent(Mesh* mesh, Material* material);

	void setMesh(Mesh* mesh);
	Mesh* getMesh() const;

	void setMaterial(Material* material);
	Material* getMaterial() const;

private:
	Mesh* m_mesh;
	Material* m_material;
};
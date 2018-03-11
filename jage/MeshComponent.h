#pragma once

#include "GameObject.h"
#include "Material.h"
#include "Shader.h"
#include "Mesh.h"

class MeshComponent
{
public:
	MeshComponent(const Mesh* mesh = nullptr, 
		const Material& material = Material());

	void setMesh(const Mesh* mesh);
	const Mesh* getMesh() const;

	void setMaterial(const Material& material);
	Material getMaterial() const;

private:
	const Mesh* m_mesh;
	Material m_material;
};
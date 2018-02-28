#pragma once

#include "GameObject.h"
#include "Material.h"
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"

class MeshObject : public GameObject
{
public:
	MeshObject(const std::string& name, const Mesh* mesh = nullptr, 
		const Material& material = Material());

	void onDraw() override;

	void setMesh(const Mesh* mesh);
	Mesh* getMesh() const;

	void setMaterial(const Material& material);
	Material getMaterial() const;

private:
	Shader* m_meshShader;

	const Mesh* m_mesh;
	Material m_material;
};
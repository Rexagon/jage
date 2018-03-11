#include "MeshComponent.h"

#include "Log.h"

MeshComponent::MeshComponent(const Mesh * mesh, const Material & material) :
	m_mesh(mesh), m_material(material)
{
}

void MeshComponent::setMesh(const Mesh * mesh)
{
	m_mesh = mesh;
}

const Mesh * MeshComponent::getMesh() const
{
	return m_mesh;
}

void MeshComponent::setMaterial(const Material & material)
{
	m_material = material;
}

Material MeshComponent::getMaterial() const
{
	return Material();
}

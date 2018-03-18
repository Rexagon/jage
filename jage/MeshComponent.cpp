#include "MeshComponent.h"

#include "Log.h"

MeshComponent::MeshComponent(Mesh * mesh, const Material& material) :
	m_mesh(mesh), m_material(material)
{
}

void MeshComponent::setMesh(Mesh * mesh)
{
	m_mesh = mesh;
}

Mesh * MeshComponent::getMesh() const
{
	return m_mesh;
}

void MeshComponent::setMaterial(const Material& material)
{
	m_material = material;
}

Material& MeshComponent::getMaterial()
{
	return m_material;
}

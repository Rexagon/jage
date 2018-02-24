#include "MeshObject.h"

#include "Log.h"

MeshObject::MeshObject(const std::string & name, const Mesh * mesh, const Material & material) :
	GameObject(name), m_mesh(mesh), m_material(material)
{
}

void MeshObject::onDraw()
{
	if (m_mesh != nullptr) {
		m_mesh->draw();
	}
}

void MeshObject::setMesh(const Mesh * mesh)
{
	m_mesh = mesh;
}

Mesh * MeshObject::getMesh() const
{
	return nullptr;
}

void MeshObject::setMaterial(const Material & material)
{
	m_material = material;
}

Material MeshObject::getMaterial() const
{
	return Material();
}

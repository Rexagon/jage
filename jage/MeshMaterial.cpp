#include "MeshMaterial.h"

#include "ShaderFactory.h"
#include "ResourceManager.h"

MeshMaterial::MeshMaterial(Texture* albedoTexture, Texture* normalsTexture) :
	Material(DEFERRED, typeid(MeshMaterial)),
	m_uvScale(vec2(1.0f, 1.0f))
{
	ShaderFactory::FromFile meshVertexSource("shaders/mesh.vert");
	ShaderFactory::FromFile meshFragmentSource("shaders/mesh.frag");
	ResourceManager::bind<ShaderFactory>("mesh_shader", meshVertexSource, meshFragmentSource);
	m_shader = ResourceManager::get<Shader>("mesh_shader");
	m_shader->setAttribute(0, "position");
	m_shader->setAttribute(1, "texCoord");
	m_shader->setAttribute(2, "normal");

	bind();
	
	m_shader->setUniform("u_albedoTexture", 0);
	m_shader->setUniform("u_normalsTexture", 1);

	m_textures.resize(2, nullptr);
	setAlbedoTexture(albedoTexture);
	setNormalsTexture(normalsTexture);
}

void MeshMaterial::bind()
{
	m_shader->bind();

	m_shader->setUniform("u_uvScale", m_uvScale);
}

void MeshMaterial::setAlbedoTexture(Texture * texture)
{
	m_textures[0] = texture;
}

Texture * MeshMaterial::getAlbedoTexture() const
{
	return m_textures[0];
}

void MeshMaterial::setNormalsTexture(Texture * texture)
{
	m_textures[1] = texture;
}

Texture * MeshMaterial::getNormalsTexture() const
{
	return m_textures[1];
}

void MeshMaterial::setUVScale(float x, float y)
{
	m_uvScale.x = x;
	m_uvScale.y = y;
}

void MeshMaterial::setUVScale(const vec2 & scale)
{
	m_uvScale = scale;
}

vec2 MeshMaterial::getUVScale() const
{
	return m_uvScale;
}

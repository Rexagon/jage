#include "MaterialManager.h"

#include "ShaderFactory.h"
#include "ResourceManager.h"

#include "RenderStateManager.h"

std::unique_ptr<Material> MaterialManager::m_defaultMaterial = nullptr;
Shader* MaterialManager::m_meshShader = nullptr;

Shader* MaterialManager::m_shadowShader = nullptr;
Shader* MaterialManager::m_lightShader = nullptr;

void MaterialManager::init()
{
	// mesh materials
	ShaderFactory::FromFile meshVertexSource("shaders/mesh.vert");
	ShaderFactory::FromFile meshFragmentSource("shaders/mesh.frag");
	ResourceManager::bind<ShaderFactory>("mesh_shader", meshVertexSource, meshFragmentSource);
	m_meshShader = ResourceManager::get<Shader>("mesh_shader");
	m_meshShader->setAttribute(0, "position");
	m_meshShader->setAttribute(1, "texCoord");
	m_meshShader->setAttribute(2, "normal");
	RenderStateManager::setCurrentShader(m_meshShader);
	m_meshShader->setUniform("u_albedoTexture", 0);
	m_meshShader->setUniform("u_normalsTexture", 1);

	m_defaultMaterial = std::make_unique<Material>(m_meshShader);
	m_defaultMaterial->setType(Material::DEFERRED);

	// light materials
	ShaderFactory::FromFile shadowVertexSource("shaders/shadow.vert");
	ShaderFactory::FromFile shadowFragmentSource("shaders/shadow.frag");
	ResourceManager::bind<ShaderFactory>("shadow_shader", shadowVertexSource, shadowFragmentSource);
	m_shadowShader = ResourceManager::get<Shader>("shadow_shader");
	m_shadowShader->setAttribute(0, "position");

	ShaderFactory::FromFile quadVertexSource("shaders/quad.vert");

	ShaderFactory::FromFile directionalLightFragmentSource("shaders/light.frag");
	ResourceManager::bind<ShaderFactory>("light_shader", quadVertexSource, directionalLightFragmentSource);
	m_lightShader = ResourceManager::get<Shader>("light_shader");
	m_lightShader->setAttribute(0, "position");
	m_lightShader->setAttribute(1, "texCoord");
	RenderStateManager::setCurrentShader(m_lightShader);
	m_lightShader->setUniform("u_albedoTexture", 0);
	m_lightShader->setUniform("u_normalsTexture", 1);
	m_lightShader->setUniform("u_positionsTexture", 2);
	m_lightShader->setUniform("u_depthTexture", 3);
	m_lightShader->setUniform("u_lightDepthTexture", 4);
}

Shader * MaterialManager::getMeshShader()
{
	return m_meshShader;
}

Material MaterialManager::createMeshMaterial(Texture * albedo, Texture * normal)
{
	Material material = *m_defaultMaterial;
	material.getTextures().push_back(albedo);
	material.getTextures().push_back(normal);
	return material;
}

Shader * MaterialManager::getShadowShader()
{
	return m_shadowShader;
}

Shader * MaterialManager::getLightShader()
{
	return m_lightShader;
}

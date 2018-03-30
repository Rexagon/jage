#include "MaterialManager.h"

#include "ShaderFactory.h"
#include "ResourceManager.h"

#include "RenderStateManager.h"

std::unique_ptr<Material> MaterialManager::m_defaultMaterial = nullptr;
std::unique_ptr<Material> MaterialManager::m_fxaaMaterial = nullptr;

Shader* MaterialManager::m_shadowShader = nullptr;
Shader* MaterialManager::m_lightShader = nullptr;

void MaterialManager::init()
{
	// mesh materials
	ShaderFactory::FromFile meshVertexSource("shaders/mesh.vert");
	ShaderFactory::FromFile meshFragmentSource("shaders/mesh.frag");
	ResourceManager::bind<ShaderFactory>("mesh_shader", meshVertexSource, meshFragmentSource);
	auto meshShader = ResourceManager::get<Shader>("mesh_shader");
	meshShader->setAttribute(0, "position");
	meshShader->setAttribute(1, "texCoord");
	meshShader->setAttribute(2, "normal");
	RenderStateManager::setCurrentShader(meshShader);
	meshShader->setUniform("u_albedoTexture", 0);
	meshShader->setUniform("u_normalsTexture", 1);

	m_defaultMaterial = std::make_unique<Material>(meshShader);
	m_defaultMaterial->setType(Material::DEFERRED);

	// post process materials

	// quad vertex shader is general for all post process shaders
	ShaderFactory::FromFile quadVertexSource("shaders/quad.vert");
	//

	ShaderFactory::FromFile fxaaFragmentSource("shaders/fxaa.frag");
	ResourceManager::bind<ShaderFactory>("fxaa_shader", quadVertexSource, fxaaFragmentSource);
	auto fxaaShader = ResourceManager::get<Shader>("fxaa_shader");
	fxaaShader->setAttribute(0, "position");
	fxaaShader->setAttribute(1, "texCoord");

	m_fxaaMaterial = std::make_unique<Material>(fxaaShader);
	m_fxaaMaterial->setType(Material::POST_PROCESS);

	// light materials
	ShaderFactory::FromFile shadowVertexSource("shaders/shadow.vert");
	ShaderFactory::FromFile shadowFragmentSource("shaders/shadow.frag");
	ResourceManager::bind<ShaderFactory>("shadow_shader", shadowVertexSource, shadowFragmentSource);
	m_shadowShader = ResourceManager::get<Shader>("shadow_shader");
	m_shadowShader->setAttribute(0, "position");

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

Material MaterialManager::createMeshMaterial(Texture * albedo, Texture * normal)
{
	Material material = *m_defaultMaterial;
	material.getTextures().push_back(albedo);
	material.getTextures().push_back(normal);
	return material;
}

Material* MaterialManager::getFxaaMaterial()
{
	return m_fxaaMaterial.get();
}

Shader * MaterialManager::getShadowShader()
{
	return m_shadowShader;
}

Shader * MaterialManager::getLightShader()
{
	return m_lightShader;
}
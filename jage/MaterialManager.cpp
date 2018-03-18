#include "MaterialManager.h"

#include "ShaderFactory.h"
#include "ResourceManager.h"

std::unique_ptr<Material> MaterialManager::m_defaultMaterial = nullptr;

std::unique_ptr<Material> MaterialManager::m_abberationMaterial = nullptr;
std::unique_ptr<Material> MaterialManager::m_fxaaMaterial = nullptr;

void MaterialManager::init()
{
	// mesh materials
	ShaderFactory::FromFile meshVertexSource("shaders/mesh.vert");
	ShaderFactory::FromFile meshFragmentSource("shaders/mesh.frag");
	ResourceManager::bind<ShaderFactory>("mesh_shader", meshVertexSource, meshFragmentSource);
	Shader* meshShader = ResourceManager::get<Shader>("mesh_shader");
	meshShader->setAttribute(0, "position");
	meshShader->setAttribute(1, "texCoord");
	meshShader->setAttribute(2, "normal");

	m_defaultMaterial = std::make_unique<Material>(meshShader);
	m_defaultMaterial->setType(Material::DEFAULT);


	// post processing materials
	ShaderFactory::FromFile quadVertexSource("shaders/quad.vert");

	ShaderFactory::FromFile fxaaFragmentSource("shaders/fxaa.frag");
	ResourceManager::bind<ShaderFactory>("fxaa_shader", quadVertexSource, fxaaFragmentSource);
	Shader* fxaaShader = ResourceManager::get<Shader>("fxaa_shader");
	fxaaShader->setAttribute(0, "position");
	fxaaShader->setAttribute(1, "texCoord");

	m_abberationMaterial = std::make_unique<Material>(fxaaShader);
	m_abberationMaterial->setType(Material::POST_PROCESS);


	ShaderFactory::FromFile abberationFragmentSource("shaders/abberation.frag");
	ResourceManager::bind<ShaderFactory>("abberation_shader", quadVertexSource, abberationFragmentSource);
	Shader* abberationShader = ResourceManager::get<Shader>("abberation_shader");
	abberationShader->setAttribute(0, "position");
	abberationShader->setAttribute(1, "texCoord");

	m_fxaaMaterial = std::make_unique<Material>(abberationShader);
	m_fxaaMaterial->setType(Material::POST_PROCESS);
}

Material MaterialManager::createDefaultMaterial(Texture * albedo, Texture * normal)
{
	Material material = *m_defaultMaterial;
	material.getTextures().push_back(albedo);
	material.getTextures().push_back(normal);
	return material;
}

Material * MaterialManager::getAbberationMaterial()
{
	return m_abberationMaterial.get();
}

Material * MaterialManager::getFxaaMaterial()
{
	return m_fxaaMaterial.get();
}

#include "AbberationMaterial.h"

#include "ResourceManager.h"
#include "ShaderFactory.h"

AbberationMaterial::AbberationMaterial() :
	Material(POST_PROCESS, typeid(AbberationMaterial)),
	m_chromaticAberration(0.05f)
{
	ShaderFactory::FromFile shaderVertexSource("shaders/quad.vert");
	ShaderFactory::FromFile shaderFragmentSource("shaders/abberation.frag");
	ResourceManager::bind<ShaderFactory>("abberation_shader", shaderVertexSource, shaderFragmentSource);
	m_shader = ResourceManager::get<Shader>("abberation_shader");
	m_shader->setAttribute(0, "position");
	m_shader->setAttribute(1, "texCoord");

	bind();
	m_shader->setUniform("u_colorTexture", 0);
}

AbberationMaterial::~AbberationMaterial()
{
}

void AbberationMaterial::bind()
{
	m_shader->bind();

	m_shader->setUniform("u_chromaticAberration", m_chromaticAberration);
}

void AbberationMaterial::setChromaticAberration(float chromaticAberration)
{
	m_chromaticAberration = chromaticAberration;
}

float AbberationMaterial::getChromaticAberration() const
{
	return m_chromaticAberration;
}

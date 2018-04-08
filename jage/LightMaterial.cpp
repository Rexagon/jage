#include "LightMaterial.h"

#include "ResourceManager.h"
#include "ShaderFactory.h"

LightMaterial::LightMaterial() :
	Material(POST_PROCESS, typeid(LightMaterial)), 
	m_albedoTextureUnit(0),
	m_normalsTextureUnit(1),
	m_specularTextureUnit(2),
	m_depthTextureUnit(3),
	m_lightDepthTextureUnit(4),
	m_areShadowsEnabled(false), m_color(1.0f, 1.0f, 1.0f),
	m_direction(1.0f, 1.0f, 1.0f), m_inversedViewProjection(1.0f), m_lightViewProjection(1.0f)
{
	ShaderFactory::FromFile lightVertexSource("shaders/quad.vert");
	ShaderFactory::FromFile lightFragmentSource("shaders/light.frag");
	ResourceManager::bind<ShaderFactory>("light_shader", lightVertexSource, lightFragmentSource);
	m_shader = ResourceManager::get<Shader>("light_shader");
	m_shader->setAttribute(0, "position");
	m_shader->setAttribute(1, "texCoord");

	bind();
	m_shader->setUniform("u_albedoTexture", m_albedoTextureUnit);
	m_shader->setUniform("u_normalsTexture", m_normalsTextureUnit);
	m_shader->setUniform("u_specularTexture", m_specularTextureUnit);
	m_shader->setUniform("u_depthTexture", m_depthTextureUnit);
	m_shader->setUniform("u_lightDepthTexture", m_lightDepthTextureUnit);
}

LightMaterial::~LightMaterial()
{
}

void LightMaterial::bind()
{
	m_shader->bind();

	m_shader->setUniform("u_areShadowsEnabled", static_cast<int>(m_areShadowsEnabled));
	m_shader->setUniform("u_color", m_color);

	m_shader->setUniform("u_direction", m_direction);
	m_shader->setUniform("u_inversedViewProjection", m_inversedViewProjection);
	m_shader->setUniform("u_lightViewProjection", m_lightViewProjection);
}

int LightMaterial::getAlbedoTextureUnit() const
{
	return m_albedoTextureUnit;
}

int LightMaterial::getNormalsTextureUnit() const
{
	return m_normalsTextureUnit;
}

int LightMaterial::getSpecularTextureUnit() const
{
	return m_specularTextureUnit;
}

int LightMaterial::getDepthTextureUnit() const
{
	return m_depthTextureUnit;
}

int LightMaterial::getLightDepthTextureUnit() const
{
	return m_lightDepthTextureUnit;
}

void LightMaterial::setShadowsEnabled(bool enabled)
{
	m_shadowCastingEnabled = enabled;
}

bool LightMaterial::areShadowsEnabled() const
{
	return m_shadowCastingEnabled;
}

void LightMaterial::setDirection(const vec3 & direction)
{
	m_direction = direction;
}

vec3 LightMaterial::getDirection() const
{
	return m_direction;
}

void LightMaterial::setColor(const vec3 & color)
{
	m_color = color;
}

vec3 LightMaterial::getColor() const
{
	return m_color;
}

void LightMaterial::setInversedViewProjection(const mat4 & inversedViewProjection)
{
	m_inversedViewProjection = inversedViewProjection;
}

mat4 LightMaterial::getInversedViewProjection() const
{
	return m_inversedViewProjection;
}

void LightMaterial::setLightViewProjection(const mat4 & lightViewProjection)
{
	m_lightViewProjection = lightViewProjection;
}

mat4 LightMaterial::getLightViewProjection() const
{
	return m_lightViewProjection;
}

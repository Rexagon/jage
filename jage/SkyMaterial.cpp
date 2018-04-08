#include "SkyMaterial.h"

#include "ResourceManager.h"
#include "ShaderFactory.h"

SkyMaterial::SkyMaterial() :
	Material(FORWARD, typeid(SkyMaterial)),
	m_rayleigh(2.0f),
	m_turbidity(2.0f),
	m_mieCoefficient(0.005f),
	m_mieDirectionalG(0.8f),
	m_luminance(1.0f),
	m_sunDirection(1.0f, 1.0f, 1.0f)
{
	setShadowCastingEnabled(false);
	setShadowReceivingEnabled(false);
	setFaceCullingEnabled(true);
	setFaceCullingSide(GL_FRONT);

	ShaderFactory::FromFile skyVertexSource("shaders/sky.vert");
	ShaderFactory::FromFile skyFragmentSource("shaders/sky.frag");
	ResourceManager::bind<ShaderFactory>("sky_shader", skyVertexSource, skyFragmentSource);
	m_shader = ResourceManager::get<Shader>("sky_shader");
	m_shader->setAttribute(0, "position");
	m_shader->setAttribute(1, "texCoords");

	bind();
}

SkyMaterial::~SkyMaterial()
{
}

void SkyMaterial::bind()
{
	m_shader->bind();

	m_shader->setUniform("u_rayleigh", m_rayleigh);
	m_shader->setUniform("u_turbidity", m_turbidity);
	m_shader->setUniform("u_mieCoefficient", m_mieCoefficient);
	m_shader->setUniform("u_mieDirectionalG", m_mieDirectionalG);
	m_shader->setUniform("u_luminance", m_luminance);

	m_shader->setUniform("u_sunDirection", m_sunDirection);
}

void SkyMaterial::setRayleigh(float rayleigh)
{
	m_rayleigh = rayleigh;
}

float SkyMaterial::getRayleigh() const
{
	return m_rayleigh;
}

void SkyMaterial::setTurbidity(float turbidity)
{
	m_turbidity = turbidity;
}

float SkyMaterial::getTurbidity() const
{
	return m_turbidity;
}

void SkyMaterial::setMieCoefficient(float mieCoefficient)
{
	m_mieCoefficient = mieCoefficient;
}

float SkyMaterial::getMieCoefficient() const
{
	return m_mieCoefficient;
}

void SkyMaterial::setMieDirectionalG(float mieDirectionalG)
{
	m_mieDirectionalG = mieDirectionalG;
}

float SkyMaterial::getMieDirectionalG() const
{
	return m_mieDirectionalG;
}

void SkyMaterial::setLuminance(float luminance)
{
	m_luminance = luminance;
}

float SkyMaterial::getLuminance() const
{
	return m_luminance;
}

void SkyMaterial::setSunDirection(const vec3 & direction)
{
	m_sunDirection = direction;
}

vec3 SkyMaterial::getSunDirection() const
{
	return m_sunDirection;
}
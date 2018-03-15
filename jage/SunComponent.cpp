#include "SunComponent.h"

SunComponent::SunComponent() :
	m_direction(1.0f, 1.0f, 1.0f),
	m_turbidity(10.0f), m_rayleigh(2.0f), 
	m_mieCoefficient(0.005f), m_mieDirectionalG(0.8f)
{
}

void SunComponent::setDirection(const vec3 & direction)
{
	m_direction = direction;
}

vec3 SunComponent::getDirection() const
{
	return m_direction;
}

void SunComponent::setTurbidity(float turbidity)
{
	m_turbidity = turbidity;
}

float SunComponent::getTurbidity() const
{
	return m_turbidity;
}

void SunComponent::setRayleigh(float rayleigh)
{
	m_rayleigh = rayleigh;
}

float SunComponent::getRayleigh() const
{
	return m_rayleigh;
}

void SunComponent::setMieCoefficient(float mieCoefficient)
{
	m_mieCoefficient = mieCoefficient;
}

float SunComponent::getMieCoefficient() const
{
	return m_mieCoefficient;
}

void SunComponent::setMieDirectionalG(float mieDirectionalG)
{
	m_mieDirectionalG = mieDirectionalG;
}

float SunComponent::getMieDirectionalG() const
{
	return m_mieDirectionalG;
}

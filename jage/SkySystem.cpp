#include "SkySystem.h"

void SkySystem::update(const float dt)
{
	updateSun();

	m_delay += dt;
	if (m_delay >= 60.0f) {
		float seconds;
		m_delay = std::modf(m_delay, &seconds);
		m_currentTime += static_cast<unsigned int>(seconds);
		m_inclination = m_currentTime / 43200.0f * glm::pi<float>();
		m_direcitonChanged = true;
	}
}

void SkySystem::setSun(std::shared_ptr<GameObject> sun)
{
	m_sunComponent = sun->getComponent<SunComponent>();
	m_direcitonChanged = true;
}

void SkySystem::setTime(unsigned int hours, unsigned int minutes)
{
	m_currentTime = hours * 24 * 60 * 60 + minutes * 60;
	m_direcitonChanged = true;
}

void SkySystem::setAzimuth(float azimuth)
{
	m_azimuth = azimuth;
	m_direcitonChanged = true;
}

float SkySystem::getAzimuth() const
{
	return m_azimuth;
}

void SkySystem::updateSun()
{
	if (m_direcitonChanged && m_sunComponent.isValid()) {
		m_direction.x = cos(m_inclination) * sin(m_azimuth);
		m_direction.y = sin(m_inclination);
		m_direction.z = cos(m_inclination) * cos(m_azimuth);
		m_direcitonChanged = false;
	}
}
